#ifndef MEMLIB_H
#define MEMLIB_H
#include <string.h>

struct Object;

typedef struct class{
    int key_len;
    char**keys;
    struct Object*values;
    char*class_name;
}class;

typedef union Obj_val{
    struct Object*o;
    char*s;//string {char,...}
    long long int*i;//ount 
    long double*f;//floap
    long double*c;//complex {re,im}
    short int*b;//boolean 
    class*cl;//class
    char*funcid;//contain a function identifier
    char*typeid;//contain a type

}Obj_val;

typedef struct Object{
    short int type;
    Obj_val val;
}Object;

enum Obj_Type{
    Obj_Obj_t,
    Obj_string_t,
    Obj_ount_t,
    Obj_floap_t,
    Obj_complex_t,
    Obj_boolean_t,
    Obj_nil_t,
    Obj_list_t,// list starts  with Object that has value of len of the list
    Obj_end_t,// can be used to make list withput putting the len at index 0
    Obj_funcid_t,
    Obj_typeid_t,
    Obj_class_t
};


extern Object end_Obj;
extern Object nil_Obj;


#endif