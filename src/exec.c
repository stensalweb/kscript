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

// define one of 'VME__SWITCHCASE' or 'VME__GOTO' to switch between switch case statements, and computed goto
//   (https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables)

// by default, use switch case statements, as these will handle malformed inputs and give a useful error
//   message if there is an error
#define VME__SWITCHCASE

// for more optimized builds, use the computed goto to jump directly to the corresponding targets
// Note that if there is incorrectly formed bytecode, this will cause a crash and there will not be a good reason
//   printed, so this is not useful for debugging
//#define VME__GOTO


#define VME_ABORT() assert(false && "Internal Virtual Machine Error");

// now, define the correct macros
// VMED = Virtual Machine Execution Dispatch, these macros have to do with internal code generation

#if defined(VME__SWITCHCASE)

#define VMED_START while (true) switch (*pc) {
#define VMED_END default: fprintf(stderr, "ERROR: in ks_vm_exec(%p), unknown instruction code '%i' encountered\n", code, (int)*pc); VME_ABORT(); break;}

#define VMED_NEXT() continue;

#define VMED_CASE_START(_bc) case _bc:
#define VMED_CASE_END VMED_NEXT()

#elif defined(VME__GOTO)

#define VMED_START {
#define VMED_END }

#define VMED_NEXT() goto *goto_targets[*pc];

#define VMED_CASE_START(_bc) lbl_##_bc:
#define VMED_CASE_END VMED_NEXT()

#else
#error No VME__ used for execution (either 'VME__SWITCHCASE' or 'VME__GOTO' should be defined)
#endif




// internal execution algorithm
int vm_exec(ks_code code) {

    // start program counter at the beginning
    ksb* pc = code->bc;

    ksb op;

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

    // start executing now
    VMED_NEXT();

    #endif

    // inner dispatch loop for consuming bytecode
    VMED_START

        VMED_CASE_START(KSB_NOOP)
            VMED_CONSUME(ksb, op);
            printf("Noop\n");

        VMED_CASE_END

        /* STACK MANIPULATION */

        VMED_CASE_START(KSB_PUSH)
            VMED_CONSUME(ksb_i32, op_i32);

            printf("Push\n");

        VMED_CASE_END

        VMED_CASE_START(KSB_DUP)
            VMED_CONSUME(ksb, op);

            printf("Dup\n");

        VMED_CASE_END

        VMED_CASE_START(KSB_POPU)
            VMED_CONSUME(ksb, op);

            printf("Popu\n");

        VMED_CASE_END


        /* CONTROL FLOW */

        VMED_CASE_START(KSB_RET)
            VMED_CONSUME(ksb, op);

            printf("Ret\n");

            // TODO: handle stack
            return 0;

        VMED_CASE_END

        VMED_CASE_START(KSB_JMP)
            VMED_CONSUME(ksb_i32, op_i32);

            printf("Jmp\n");

            // increment program counter
            //pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_JMPT)
            VMED_CONSUME(ksb_i32, op_i32);

            printf("Jmpt\n");

            // increment program counter
            //pc += op_i32.arg;

        VMED_CASE_END


        VMED_CASE_START(KSB_JMPF)
            VMED_CONSUME(ksb_i32, op_i32);

            printf("Jmpf\n");

            // increment program counter
            //pc += op_i32.arg;

        VMED_CASE_END



    VMED_END


    /*while (true) {
        ksb op = *pc;
    }*/


}






