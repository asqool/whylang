#include "../include/interpreter.h"
#include "../include/instruction.h"
#include <stdlib.h>

//builtin libs
#include "../sulfur_libs/blt_libs/std.h"
#include "../include/operation.h"
#include "../include/utilities.h"



memory MEMORY;


Object**STACK;//end with an ar
char***STACK_KEY;//end with an

long long int STACK_len;
long long int*sub_STACK_len;//

Funcdef*FUNCDEFS;
long long int FUNCDEFS_len;

CLASSDEF*CLASSDEFS;
long long int CLASSDEF_len;

ref_counter*REF_COUNTS;
long long int REF_COUNT_len;



//void check_libs(){
//    for(int i=0;i<LIBS_len;i++){
//        if(LIBS[i].name[0]==" "[0]){
//            FUNCDEFS_len+=LIBS[i].nbr_funcs;
//            FUNCDEFS=realloc(FUNCDEFS,sizeof(Funcdef)*FUNCDEFS_len);
//            for(int k=0;k<LIBS[i].nbr_funcs;k++){
//                FUNCDEFS[FUNCDEFS_len-LIBS[i].nbr_funcs+k]=LIBS[i].funcs[k];
//
//            }
//        }
//    }
//}


void init_memory(){
    MEMORY.values=malloc(sizeof(Object));
    MEMORY.keys=malloc(sizeof(char *));
    MEMORY.len=0;
}


void init_stack(){
    STACK=malloc(sizeof(Object*));
    STACK_KEY=malloc(sizeof(char**));
    STACK_len=0;
    sub_STACK_len=malloc(sizeof(int));
    *sub_STACK_len=0;
}

void init_funcdefs(){
    FUNCDEFS=malloc(sizeof(Funcdef));
    FUNCDEFS_len=0;
}

void init_classdefs(){
    CLASSDEF_len=0;
    CLASSDEFS=malloc(sizeof(CLASSDEF));
}



void init_garbage_collect(){
    REF_COUNT_len=0;
    REF_COUNTS=malloc(sizeof(ref_counter));
}

void init_libs(){
    MEMORY=init_std(MEMORY);
}



Object eval_Ast(Ast*x){
    if(x->type==Ast_funccall_t){
        int n=-1;
        
        for(int i=0;i<MEMORY.len;i++){

            if(!strcmp(MEMORY.keys[i],x->root.fun->name)&& MEMORY.values[i].type==Obj_funcid_t){
                n=i;
            }
        }
        if(n==-1){
            printf("function '%s' doesnt exit",x->root.fun->name);
            exit(1);
        }
        
        if(MEMORY.values[n].val.funcid->is_builtin){
            Object*a=malloc(sizeof(Object)*x->root.fun->nbr_arg);
            for(int i=0;i<x->root.fun->nbr_arg;i++){
                a[i]=eval_Ast(&x->root.fun->args[i]);
            }
            return (*MEMORY.values[n].val.funcid->func_p)(a,x->root.fun->nbr_arg);
        }
    }
    if(x->left==NULL && x->right==NULL&& !x->isAst){
        if(x->type == Ast_varcall_t){
            int n=-1;
            for(int i=0;i<MEMORY.len;i++){
                if(!strcmp(MEMORY.keys[i],x->root.varcall)){
                    n=i;
                }
            }
            if(n==-1){
                printf("ERROR var '%s' doesnt exist",x->root.varcall);
            }
            return MEMORY.values[n];
        }
        if(x->type == Ast_object_t){
            return *(x->root.obj);
        }
        else{
            printf("type:%d:",x->type);
            printf("ERROR in Ast");
            exit(-1);
        }
    }
    if(x->isAst){
        if(x->type==Ast_add_t){
            if(x->left!=NULL && x->right!=NULL){
                Object a=eval_Ast(x->left);
                Object b=eval_Ast(x->right);
                return add(a,b);
            }
        }
        if(x->type==Ast_le_t){
            if(x->left!=NULL && x->right!=NULL){
                Object a=eval_Ast(x->left);
                Object b=eval_Ast(x->right);
                return less(a,b);
            }
        }
        if(x->type==Ast_ge_t){
            if(x->left!=NULL && x->right!=NULL){
                Object a=eval_Ast(x->left);
                Object b=eval_Ast(x->right);
                return greater(a,b);
            }
        }
        if(x->type==Ast_sub_t){
            if(x->left!=NULL && x->right!=NULL){
                Object a=eval_Ast(x->left);
                Object b=eval_Ast(x->right);
                return sub(a,b);
            }
            if(x->left==NULL && x->right!=NULL){
                Object b=eval_Ast(x->right);
                return negate(b);
            }
        }
        if(x->type==Ast_pow_t){
            if(x->left!=NULL && x->right!=NULL){
                Object a=eval_Ast(x->left);
                Object b=eval_Ast(x->right);
                return _pow(a,b);
            }
        }
        if(x->type==Ast_eq_t){
            if(x->left!=NULL && x->right!=NULL){
                Object a=eval_Ast(x->left);
                Object b=eval_Ast(x->right);
                return eq(a,b);
            }
        }
        if(x->type==Ast_div_t){
            if(x->left!=NULL && x->right!=NULL){
                Object a=eval_Ast(x->left);
                Object b=eval_Ast(x->right);
                return _div(a,b);
            }
        }
        if(x->type==Ast_mul_t){
            if(x->left!=NULL && x->right!=NULL){
                Object a=eval_Ast(x->left);
                Object b=eval_Ast(x->right);
                return mul(a,b);
            }
        }
    }
}

//return 1 if the object already exists
//return 0 if it doesnt exists
//each object of a list will be add_red-ed automaticly even the first element wich is the size of the list
//class are not supported yet
int add_ref(Object o){
    int find=-1;
    for(int i=0;i<REF_COUNT_len;i++){
        if(REF_COUNTS[i].pointer==get_obj_pointer(o)){
            find=i;
        }
    }
    if(find!=-1){
        REF_COUNTS[find].count++;
        return 1;
    }
    if(o.type=Obj_class_t){
        return 0;
    }
    if(o.type=Obj_list_t){
        //REF_COUNT_len++;
        //REF_COUNTS=realloc(REF_COUNTS,sizeof(ref_counter)*REF_COUNT_len);
        //REF_COUNTS[REF_COUNT_len-1].count=1;
        //REF_COUNTS[REF_COUNT_len-1].type=Obj_list_t;
        //REF_COUNTS[REF_COUNT_len-1].pointer->li=o.val.li;
        //int len=o.val.li->len;//faut le remplacer par [0]
        //for(int i=0;i<len;i++){
        //    add_ref(o.val.li->elements[i]);
        //}
        return 0;
    }
    REF_COUNT_len++;
    REF_COUNTS=realloc(REF_COUNTS,sizeof(ref_counter)*REF_COUNT_len);
    REF_COUNTS[REF_COUNT_len-1].count=1;
    REF_COUNTS[REF_COUNT_len-1].type=o.type;
    //REF_COUNTS[REF_COUNT_len-1].pointer=get_obj_pointer(o); faut faire switch case pointer.type
    return 0;
}

int execute(Instruction*code,char*file_name,int len){
    int p=0;
    //check_libs();
    while(p<len){
        if(code[p].type==inst_pass_t){
            p++;
            continue;
        }
        if(code[p].type==inst_varset_t){
            for(int i=0;i<MEMORY.len;i++){
                if(!strcmp(MEMORY.keys[i],code[p].value.vs->name)){
                    printf("error redefinition of var please delete var befor");
                    exit(1);
                }
            }
            MEMORY.len++;
            MEMORY.values=realloc_c(MEMORY.values,sizeof(Object)*(MEMORY.len-1),sizeof(Object)*MEMORY.len);
            MEMORY.keys=realloc_c(MEMORY.keys,sizeof(char*)*(MEMORY.len-1),sizeof(char*)*MEMORY.len);
            MEMORY.keys[MEMORY.len-1]=malloc(sizeof(char)*(1+strlen(code[p].value.vs->name)));
            strcpy(MEMORY.keys[MEMORY.len-1],code[p].value.vs->name);
            //print_ast(*code[p].value.vs->val);
            //printf("\n");
            Object o=eval_Ast(code[p].value.vs->val);//faut le raplce par Object pas Object*
            add_ref(o);
            MEMORY.values[MEMORY.len-1]=o;
            p++;
            continue;       

        }
        if(code[p].type==inst_new_varset_t){
            int n=-1;
            for(int i=0;i<MEMORY.len;i++){
                if(!strcmp(MEMORY.keys[i],code[p].value.vs->name)){
                    n=i;
                    break;
                }
            }
            MEMORY.values[n]=eval_Ast(code[p].value.vs->val);
            p++;         
        }
        if(code[p].type==inst_if_t){
            Object condition=eval_Ast(code[p].value.i->condition);
            if(*(std_bool(&condition,1).val.b)){
                p++;
                continue;
            }
            p=code[p].value.i->endif_p+1;
            continue;
        }
        if(code[p].type==inst_elif_t){
            Object condition=eval_Ast(code[p].value.el->condition);
            if(*(std_bool(&condition,1).val.b)){
                p++;
                continue;
            }
            p=code[p].value.el->endif_p+1;
            continue;
        }
        if(code[p].type==inst_else_t){
            p++;
            continue;
        }
        if(code[p].type==inst_endif){
            p=code[p].value.endifelse+1;
            continue;

        }
        if(code[p].type==inst_endifelse){
            p++;
            continue;
        }
        if(code[p].type==inst_return_t){
            Object*x=malloc(sizeof(Object));
            *x=eval_Ast(code[p].value.ret);
            p++;
            continue;
        }

        if(code[p].type==inst_expr_t){
            eval_Ast(code[p].value.expr);
            p++;
            continue;
        }
        if(code[p].type==inst_section_t){
            p++;
        }
        if(code[p].type==inst_goto_t){
            int n=-1;
            //search down
            for(int i=p+1;i<len;i++){
                if(code[i].type==inst_section_t){
                    if(!strcmp(code[p].value.goto_sec,code[i].value.section)){
                        n=i;
                        break;
                    }
                }
            }
            if(n==-1){
                //search up
                for(int i=p-1;i>=0;i--){
                    if(code[i].type==inst_section_t){
                        if(!strcmp(code[p].value.goto_sec,code[i].value.section)){
                            n=i;
                            break;
                        }
                    }
                }
            }
            if(n==-1){
                printf("section %s doesnt exists",code[p].value.goto_sec);
                exit(1);
            }
            p=n;
            continue;
        }
        if(code[p].type==inst_for_t){
            Object start=eval_Ast(code[p].value.fo->start);
            start=std_ount(&start,1);
            Object end=eval_Ast(code[p].value.fo->end);
            end=std_ount(&end,1);
            if(start.type==Obj_nil_t){
                printf("ERROR cant convert the value of start to ount in for");
                exit(1);
            }
            if(end.type==Obj_nil_t){
                printf("ERROR cant convert the value of end to ount in for");
                exit(1);
            }

            int n=-1;
            for(int i=0;i<MEMORY.len;i++){
                if(!strcmp(MEMORY.keys[i],code[p].value.fo->var_name)){
                    n=i;
                    break;
                }
            }
            if(n==-1){
                MEMORY.len++;
                MEMORY.values=realloc_c(MEMORY.values,sizeof(Object)*(MEMORY.len-1),sizeof(Object)*MEMORY.len);
                MEMORY.keys=realloc_c(MEMORY.keys,sizeof(char*)*(MEMORY.len-1),sizeof(char*)*MEMORY.len);
                MEMORY.keys[MEMORY.len-1]=malloc(sizeof(char)*(1+strlen(code[p].value.fo->var_name)));
                strcpy(MEMORY.keys[MEMORY.len-1],code[p].value.fo->var_name);
                MEMORY.values[MEMORY.len-1].type=Obj_ount_t;
                MEMORY.values[MEMORY.len-1].val.i=malloc(sizeof(long long int));
                *MEMORY.values[MEMORY.len-1].val.i=*start.val.i;
                add_ref(MEMORY.values[MEMORY.len-1]);
            }
            else{
                MEMORY.values[n].type=Obj_ount_t;
                MEMORY.values[n].val.i=malloc(sizeof(long long int));
                *MEMORY.values[n].val.i=*start.val.i;
                add_ref(MEMORY.values[n]);

            }

            if(*start.val.i==*end.val.i){
                p=code[p].value.fo->endfor+1;
                continue;
            }
            p++;
        }
        if(code[p].type==inst_endfor_t){
            int for_p=code[p].value.endfor;
            Object start=eval_Ast(code[for_p].value.fo->start);
            start=std_ount(&start,1);
            Object end=eval_Ast(code[for_p].value.fo->end);
            end=std_ount(&end,1);

            //positive for 
            if(*start.val.i<*end.val.i){
                int n=-1;
                for(int i=0;i<MEMORY.len;i++){
                    if(!strcmp(MEMORY.keys[i],code[for_p].value.fo->var_name)){
                        n=i;
                        break;
                    }
                }
                if(n==-1){
                    p++;
                    continue;
                }
                
                MEMORY.values[n]=std_ount(&MEMORY.values[n],1);
                if(MEMORY.values[n].type==Obj_nil_t){
                    printf("ERROR in for cant convert loop var to ount");
                    exit(1);
                }
                (*MEMORY.values[n].val.i)++;

                if(*end.val.i>*MEMORY.values[n].val.i){

                    p=for_p+1;
                    continue;
                }
                else{
                    p++;
                    continue;
                }
            }
            else{
                int n=-1;
                for(int i=0;i<MEMORY.len;i++){
                    if(!strcmp(MEMORY.keys[i],code[for_p].value.fo->var_name)){
                        n=i;
                        break;
                    }
                }
                if(n==-1){
                    p++;
                    continue;
                }
                
                MEMORY.values[n]=std_ount(&MEMORY.values[n],1);
                if(MEMORY.values[n].type==Obj_nil_t){
                    printf("ERROR in for cant convert loop var to ount");
                    exit(1);
                }
                (*MEMORY.values[n].val.i)--;

                if(*end.val.i<*MEMORY.values[n].val.i){

                    p=for_p+1;
                    continue;
                }
                else{
                    p++;
                    continue;
                }
            }
            
        }
        if(code[p].type==inst_while_t){
            Object condition=eval_Ast(code[p].value.wh->condition);
            if(*(std_bool(&condition,1).val.b)){
                p++;
                continue;
            }
            else{
                p=code[p].value.wh->endwhile+1;
                continue;
            }
        }
        if(code[p].type==inst_endwhile_t){
            int while_p=code[p].value.endwhile;
            Object condition=eval_Ast(code[while_p].value.wh->condition);
            if(*(std_bool(&condition,1).val.b)){
                p=code[p].value.endwhile+1;
                continue;
            }
            else{
                p++;
            }
        }
    }
    
    return 0;
}