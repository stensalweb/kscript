/* Enum.c - implementation of the enum class
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// make an enum value
static ks_Enum make_Enum_val(ks_type enumtype, ks_str name, ks_int val) {
    ks_Enum self = KS_ALLOC_OBJ(ks_Enum);
    KS_INIT_OBJ(self, enumtype);

    self->name = (ks_str)KS_NEWREF(name);
    self->enum_val = (ks_int)KS_NEWREF(val);

    return self;
}

// Create a new enumeration type, with given values
// NOTE: Returns new reference, or NULL if an error was thrown
ks_type ks_Enum_create_c(const char* name, ks_enumval_c* enumvals) {
    ks_type enumtype = KS_ALLOC_OBJ(ks_type);

    // create num -> name mappings as well as name -> num mapping
    ks_dict map_num2name = ks_dict_new(0, NULL), map_name2num = ks_dict_new(0, NULL);


    // initialize the type
    ks_type_init_c(enumtype, name, ks_T_Enum, KS_KEYVALS(

        {"_enum_num2name",         (ks_obj)map_num2name},
        {"_enum_name2num",         (ks_obj)map_name2num},
    ));

    while (enumvals->name) {

        // create kscript objects
        ks_str name = ks_str_new(enumvals->name);
        ks_int num = ks_int_new(enumvals->val);

        ks_Enum val = make_Enum_val(enumtype, name, num);

        ks_dict_set(map_num2name, (ks_obj)num, (ks_obj)val);
        ks_dict_set(map_name2num, (ks_obj)name, (ks_obj)val);
        ks_dict_set(enumtype->attr, (ks_obj)name, (ks_obj)val);

        // done with them
        KS_DECREF(name);
        KS_DECREF(num);

        // advance to next one
        enumvals++;
    }

    return enumtype;

}

// Construct a new enumeration value
// NOTE: Returns new reference, or NULL if an error was thrown
ks_Enum ks_Enum_new(ks_type enumtype, ks_str val) {
    ks_str key = ks_str_new("_enum_name2num");
    ks_dict name2num = (ks_dict)ks_type_get(enumtype, key);
    KS_DECREF(key);
    if (!name2num) return (ks_Enum)ks_throw(ks_T_InternalError, "Enumeration did not have attribute `_enum_name2num`!");

    // get it
    ks_Enum ret = (ks_Enum)ks_dict_get(name2num, (ks_obj)val);

    if (!ret) return ks_throw(ks_T_KeyError, "Enumeration '%S' had no member: %R", enumtype, val);
    return ret;
}


// Construct a new enumeration value
// NOTE: Returns new reference, or NULL if an error was thrown
ks_Enum ks_Enum_new_c(ks_type enumtype, char* cstr) {
    ks_str val = ks_str_new(cstr);
    ks_Enum ret = ks_Enum_new(enumtype, val);
    KS_DECREF(val);
    return ret;
}


// Construct a new enumeration value
// NOTE: Returns new reference, or NULL if an error was thrown
ks_Enum ks_Enum_new_i(ks_type enumtype, ks_int val) {
    ks_str key = ks_str_new("_enum_num2name");
    ks_dict num2name = (ks_dict)ks_type_get(enumtype, key);
    KS_DECREF(key);
    if (!num2name) return (ks_Enum)ks_throw(ks_T_InternalError, "Enumeration did not have attribute `_enum_num2name`!");

    // get it
    ks_Enum ret = (ks_Enum)ks_dict_get(num2name, (ks_obj)val);

    if (!ret) return ks_throw(ks_T_KeyError, "Enumeration '%S' had no member: %R", enumtype, val);
    return ret;
}

// Construct a new enumeration value
// NOTE: Returns new reference, or NULL if an error was thrown
ks_Enum ks_Enum_new_ic(ks_type enumtype, int64_t val) {
    ks_int oval = ks_int_new(val);
    ks_Enum ret = ks_Enum_new_i(enumtype, oval);
    KS_DECREF(oval);
    return ret;
}


// Enum.create(name, vals) - create a new enumeration type
static KS_TFUNC(Enum, create) {
    ks_str name;
    ks_dict vals;
    KS_GETARGS("name:* vals:*", &name, ks_T_str, &vals, ks_T_dict)

    // create empty type
    ks_type enumtype = ks_Enum_create_c(name->chr, KS_ENUMVALS());

    // get dictionaries required
    ks_str attr = ks_str_new("_enum_name2num");
    ks_dict name2num = (ks_dict)ks_type_get(enumtype, attr);
    KS_DECREF(attr);
    if (!name2num) return ks_throw(ks_T_InternalError, "Enumeration did not have attribute `_enum_name2num`!");

    attr = ks_str_new("_enum_num2name");
    ks_dict num2name = (ks_dict)ks_type_get(enumtype, attr);
    KS_DECREF(attr);
    if (!num2name) {
        KS_DECREF(name2num);
        return ks_throw(ks_T_InternalError, "Enumeration did not have attribute `_enum_num2name`!");
    }

    // iterate through given values
    struct ks_dict_citer cit = ks_dict_citer_make(vals);
    ks_obj key, val;
    ks_hash_t hash;
    while (ks_dict_citer_next(&cit, &key, &val, &hash)) {
        // add it
        if (key->type != ks_T_str) {
            ks_throw(ks_T_ArgError, "Name for enum member %R was not a string (strings are required)", key);
            KS_DECREF(key);
            KS_DECREF(val);
            KS_DECREF(enumtype);
            return NULL;
        }

        ks_int intval = ks_num_get_int(val);
        if (!intval) {
            ks_throw(ks_T_ArgError, "Value for enum member %R was not an integer value (integers are required)", val);
            KS_DECREF(key);
            KS_DECREF(val);
            KS_DECREF(enumtype);
            return NULL;
        }

        ks_Enum enumval = make_Enum_val(enumtype, (ks_str)key, intval);

        // set references
        ks_dict_set(name2num, key, (ks_obj)enumval);
        ks_dict_set(num2name, (ks_obj)intval, (ks_obj)enumval);
        ks_dict_set(enumtype->attr, key, (ks_obj)enumval);

        KS_DECREF(intval);

        // get rid of references to dictionary iterators
        KS_DECREF(key);
        KS_DECREF(val);
    }


    return (ks_obj)enumtype;
}

// Enum.__getitem__(typ, name)
static KS_TFUNC(Enum, getitem) {
    ks_type typ;
    ks_obj name;
    KS_GETARGS("typ:* name", &typ, ks_T_type, &name)

    if (!ks_type_issub(typ, ks_T_Enum)) {
        return ks_throw(ks_T_TypeError, "Expected argument #0 to `Enum.__getitem__` to be an enumeration type, but got: %S", typ);
    }

    if (name->type == ks_T_str) {
        // get string
        return (ks_obj)ks_Enum_new(typ, (ks_str)name);
    } else if (ks_num_is_integral(name)) {
        ks_int num = ks_num_get_int(name);
        if (!num) return NULL;

        return (ks_obj)ks_Enum_new_i(typ, num);
    } else {
        return ks_throw(ks_T_TypeError, "Expected 'name' to be either an 'int', or 'str', but got '%T'", name);
    }
}



// Enum.__free__(self) - free obj
static KS_TFUNC(Enum, free) {
    ks_Enum self;
    KS_GETARGS("self:*", &self, ks_T_Enum)

    KS_DECREF(self->name);
    KS_DECREF(self->enum_val);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// Enum.__int__(self) - to integer
static KS_TFUNC(Enum, int) {
    ks_Enum self;
    KS_GETARGS("self:*", &self, ks_T_Enum)

    return KS_NEWREF(self->enum_val);
}

// Enum.__str__(self) - to string
static KS_TFUNC(Enum, str) {
    ks_Enum self;
    KS_GETARGS("self:*", &self, ks_T_Enum)

    return (ks_obj)ks_fmt_c("%T.%S", self, self->name);
}

// operators
KST_NUM_OPFS(Enum)


/* export */

KS_TYPE_DECLFWD(ks_T_Enum);

void ks_init_T_Enum() {
    ks_type_init_c(ks_T_Enum, "Enum", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(Enum_free_, "Enum.__free__(self)")},
        {"__new__",                (ks_obj)ks_cfunc_new_c(Enum_getitem_, "Enum.__new__(typ, name)")},

        {"__str__",                (ks_obj)ks_cfunc_new_c(Enum_str_, "Enum.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c(Enum_str_, "Enum.__repr__(self)")},

        {"__int__",                (ks_obj)ks_cfunc_new_c(Enum_int_, "Enum.__int__(self)")},

        {"create",                 (ks_obj)ks_cfunc_new_c(Enum_create_, "Enum.create(name, vals)")},

        KST_NUM_OPKVS(Enum)

    ));


}
