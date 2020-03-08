/* exec.c - implementation of the internal bytecode execution engine for kscript
 *
 * 
 * There is a switch-case, and a computed-goto configuration for the execution engine
 * 
 * 
 * 
 * 
 * More on computed goto: https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_vm);

// default virtual machine
ks_vm ks_vm_default = NULL;


// construct new VM
ks_vm ks_vm_new() {
    ks_vm self = KS_ALLOC_OBJ(ks_vm);
    KS_INIT_OBJ(self, ks_type_vm);

    // initialize to empty
    self->globals = ks_dict_new(0, NULL);

    self->stk = ks_list_new(0, NULL);

    return self;
}

void ks_type_vm_init() {
    KS_INIT_TYPE_OBJ(ks_type_vm, "vm");

    // set the default
    ks_vm_default = ks_vm_new();

    ks_dict_set_cn(ks_vm_default->globals, (ks_dict_ent_c[]){
        {"int", KS_NEWREF(ks_type_int)},
        {"str", KS_NEWREF(ks_type_str)},
        {"list", KS_NEWREF(ks_type_list)},

        {"hash", KS_NEWREF(ks_F_hash)},
        {"repr", KS_NEWREF(ks_F_repr)},
        {"print", KS_NEWREF(ks_F_print)},

        {NULL, NULL}
    });


    /*ks_type_set_cn(ks_type_code, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(code_str_)},
        {"__free__", (ks_obj)ks_cfunc_new(code_free_)},
        {NULL, NULL}   
    });*/
}



// define one of 'VME__SWITCHCASE' or 'VME__GOTO' to switch between switch case statements, and computed goto
//   (https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables)

// by default, use switch case statements, as these will handle malformed inputs and give a useful error
//   message if there is an error
#define VME__SWITCHCASE

// for more optimized builds, use the computed goto to jump directly to the corresponding targets
// Note that if there is incorrectly formed bytecode, this will cause a crash and there will not be a good reason
//   printed, so this is not useful for debugging
//#define VME__GOTO


#define VME_ASSERT(...) assert(__VA_ARGS__)
#define VME_ABORT() assert(false && "Internal Virtual Machine Error");

// now, define the correct macros
// VMED = Virtual Machine Execution Dispatch, these macros have to do with internal code generation

#if defined(VME__SWITCHCASE)

#define VMED_START while (true) switch (*pc) {
#define VMED_END default: fprintf(stderr, "ERROR: in ks_vm_exec(%p), unknown instruction code '%i' encountered\n", code, (int)*pc); VME_ABORT(); break;}

#define VMED_NEXT() continue;

#define VMED_CASE_START(_bc) case _bc: {
#define VMED_CASE_END VMED_NEXT() }

#elif defined(VME__GOTO)

#define VMED_START { VMED_NEXT();
#define VMED_END }

#define VMED_NEXT() goto *goto_targets[*pc];

#define VMED_CASE_START(_bc) lbl_##_bc: {
#define VMED_CASE_END VMED_NEXT() }

#else
#error No VME__ used for execution (either 'VME__SWITCHCASE' or 'VME__GOTO' should be defined)
#endif


// the default VM
ks_vm ks_vm_default;

// internal execution algorithm
ks_obj vm_exec(ks_vm vm, ks_code code) {

    // start program counter at the beginning
    ksb* pc = code->bc;

    ksb op;

    // temporary variables
    int i, j;

    // temporary variable for a bytecode with a 32 bit integer argument
    ksb_i32 op_i32;

    // consume a structure from the program counter
    #define VMED_CONSUME(_type, _var) { \
        _var = *(_type*)(pc);           \
        pc += sizeof(_type);            \
    }

    // if we are using computed goto, we need to innitialize the labels
    #ifdef VME__GOTO

    // initialize the goto target
    #define GOTO_TARGET(_bc) [_bc] = &&lbl_##_bc,

    static void* goto_targets[] = {
        GOTO_TARGET(KSB_NOOP)
        
        GOTO_TARGET(KSB_PUSH)
        GOTO_TARGET(KSB_DUP)
        GOTO_TARGET(KSB_POPU)

        GOTO_TARGET(KSB_RET)
        GOTO_TARGET(KSB_JMP)
        GOTO_TARGET(KSB_JMPT)
        GOTO_TARGET(KSB_JMPF)
    
    
    };

    #endif

    // inner dispatch loop for consuming bytecode
    VMED_START

        VMED_CASE_START(KSB_NOOP)
            VMED_CONSUME(ksb, op);

        VMED_CASE_END

        /* STACK MANIPULATION */

        VMED_CASE_START(KSB_PUSH)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_list_push(vm->stk, code->v_const->elems[op_i32.arg]);

        VMED_CASE_END

        VMED_CASE_START(KSB_DUP)
            VMED_CONSUME(ksb, op);

        VMED_CASE_END

        VMED_CASE_START(KSB_POPU)
            VMED_CONSUME(ksb, op);

            ks_list_popu(vm->stk);

        VMED_CASE_END


        /* CONTROL FLOW */

        VMED_CASE_START(KSB_CALL)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_obj args[8];
            ks_list_popn(vm->stk, op_i32.arg, args);

            ks_obj ret = ks_call(args[0], op_i32.arg - 1, &args[1]);
            if (!ret) {
                // err
                goto EXC;
            }
            
            ks_list_push(vm->stk, ret);

            for (i = 0; i < op_i32.arg; ++i) KS_DECREF(args[i]);

        VMED_CASE_END

        VMED_CASE_START(KSB_RET)
            VMED_CONSUME(ksb, op);

            // return TOS
            return ks_list_pop(vm->stk);

        VMED_CASE_END

        VMED_CASE_START(KSB_JMP)
            VMED_CONSUME(ksb_i32, op_i32);

            // increment program counter
            pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_JMPT)
            VMED_CONSUME(ksb_i32, op_i32);

            // take the top item off
            ks_obj cond = ks_list_pop(vm->stk);

            if (cond == KSO_TRUE) {
                // do jump
                pc += op_i32.arg;
            } else if (cond == KSO_FALSE) {
                // don't jump

            } else {
                // we need to calculate it, default to true
                pc += op_i32.arg;
                KS_DECREF(cond);
            }

        VMED_CASE_END


        VMED_CASE_START(KSB_JMPF)
            VMED_CONSUME(ksb_i32, op_i32);

            // take the top item off
            ks_obj cond = ks_list_pop(vm->stk);

            if (cond == KSO_TRUE) {
                // don't jump
            } else if (cond == KSO_FALSE) {
                // do jump
                pc += op_i32.arg;
            } else {
                // we need to calculate it, default to true
                pc += op_i32.arg;
                KS_DECREF(cond);
            }


        VMED_CASE_END


        /* VALUE LOOKUP */

        VMED_CASE_START(KSB_LOAD)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str name = (ks_str)code->v_const->elems[op_i32.arg];
            VME_ASSERT(name->type == ks_type_str && "load [name] : 'name' must be a string");

            ks_obj val = ks_dict_get(vm->globals, name->v_hash, (ks_obj)name);
            if (!val) {
                printf("ERRname\n");
            }
            ks_list_push(vm->stk, val);
            KS_DECREF(val);


            // increment program counter
            //pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_STORE)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str name = (ks_str)code->v_const->elems[op_i32.arg];
            VME_ASSERT(name->type == ks_type_str && "store [name] : 'name' must be a string");

            // get top
            ks_obj val = vm->stk->elems[vm->stk->len - 1];

            // set it in the globals
            // TODO: add local variables too
            ks_dict_set(vm->globals, name->v_hash, (ks_obj)name, val);

            // increment program counter
            //pc += op_i32.arg;

        VMED_CASE_END



        VMED_CASE_START(KSB_LOAD_ATTR)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str attr = (ks_str)code->v_const->elems[op_i32.arg];
            VME_ASSERT(attr->type == ks_type_str && "load_attr [name] : 'name' must be a string");

            ks_obj obj = ks_list_pop(vm->stk);

            ks_obj val = ks_F_getattr->func(2, (ks_obj[]){ obj, (ks_obj)attr });
            if (!val) goto EXC;

            ks_list_push(vm->stk, val);
            KS_DECREF(obj);

            // increment program counter
            //pc += op_i32.arg;

        VMED_CASE_END


        VMED_CASE_START(KSB_STORE_ATTR)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str attr = (ks_str)code->v_const->elems[op_i32.arg];
            VME_ASSERT(attr->type == ks_type_str && "store_attr [name] : 'name' must be a string");

            assert(vm->stk->len >= 2 && "store_attr : Not enough items on stack!");


            ks_obj val = ks_list_pop(vm->stk);
            ks_obj obj = ks_list_pop(vm->stk);

            ks_obj ret = ks_F_setattr->func(3, (ks_obj[]){ obj, (ks_obj)attr, val });
            if (!ret) goto EXC;


            // ignore it
            KS_DECREF(ret);

            ks_list_push(vm->stk, val);
            KS_DECREF(obj);

            // increment program counter
            //pc += op_i32.arg;

        VMED_CASE_END


        // template for a binary operator case
        // 3rd argument is the 'extra code' to be ran to possibly shortcut it
        #define T_BOP_CASE(_bop,  _str, _func, ...) { \
            VMED_CASE_START(_bop) \
                VMED_CONSUME(ksb, op); \
                ks_obj R = ks_list_pop(vm->stk); \
                ks_obj L = ks_list_pop(vm->stk); \
                { __VA_ARGS__ } \
                ks_obj ret = (_func->func)(2, (ks_obj[]){L, R}); \
                if (!ret) goto EXC; \
                ks_list_push(vm->stk, ret); \
                KS_DECREF(L); KS_DECREF(R); KS_DECREF(ret); \
            VMED_CASE_END \
        }

        // implement all the operators
        T_BOP_CASE(KSB_BOP_ADD, "+", ks_F_add, {});
        T_BOP_CASE(KSB_BOP_SUB, "-", ks_F_sub, {});
        T_BOP_CASE(KSB_BOP_MUL, "*", ks_F_mul, {});
        T_BOP_CASE(KSB_BOP_DIV, "/", ks_F_div, {});
        T_BOP_CASE(KSB_BOP_MOD, "%", ks_F_mod, {});
        T_BOP_CASE(KSB_BOP_POW, "**", ks_F_pow, {});


        T_BOP_CASE(KSB_BOP_LT, "<", ks_F_lt, {});
        T_BOP_CASE(KSB_BOP_LE, "<=", ks_F_le, {});
        T_BOP_CASE(KSB_BOP_GT, ">", ks_F_gt, {});
        T_BOP_CASE(KSB_BOP_GE, ">=", ks_F_ge, {});
        T_BOP_CASE(KSB_BOP_EQ, "==", ks_F_eq, {});
        T_BOP_CASE(KSB_BOP_NE, "!=", ks_F_ne, {});



    VMED_END


    EXC: ;
    // handle exception here
    ks_obj exc = ks_catch();

    ks_error("EXC: %S", exc);

    /*while (true) {
        ksb op = *pc;
    }*/

    return NULL;

}






