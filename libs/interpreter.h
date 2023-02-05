#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "parser.h"
#include "memlib.h"

extern Object*MEMORY;//array of Object ended with end_obj its len is memory_len
extern char**MEMORY_key;//array of string ended with "-2.3" string
extern Object**STACK;//end with an array that start with 2 end_obj its len is stack_len
extern char***STACK_KEY;//end with an array that start with a string "-1.2"

extern long long int MEMORY_len;
extern long long int STACK_len;
extern long long int*sub_STACK_len;//list of int its len is stack_len

extern Instruction**FUNCDEFS;
extern char**FUNCDEFS_NAME;
extern long long int FUNCDEFS_len;

void init_memory();

void init_stack();

void init_funcdefs();


int execute(Instruction*code,char*file_name);


#endif