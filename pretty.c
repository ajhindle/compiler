#include <stdio.h>
#include "ast.h"

extern void report_error_and_exit(const char *msg);

const int INDENT = 10;

void print_proc(FILE *fp, int indent, Proc proc);
void print_header(FILE *fp, int indent, Header header);
void print_param(FILE *fp, Param param);
void print_decls(FILE *fp, Decls decls);
void print_type(FILE *fp, VType type);
void print_varnames(FILE *fp, VarNames varnames);
void print_statements(FILE *fp, int indent, Stmts stmts);

void
pretty_prog(FILE *fp, Program prog) {

    /* report_error_and_exit("Unable to pretty-print"); */

    Procs   next_procs;
    Proc    curr_proc;

    curr_proc = prog->procs->p_first;
    next_procs = prog->procs->p_rest;

    while(curr_proc != NULL) {
        print_proc(fp, INDENT, curr_proc);
        if (next_procs != NULL) {
            curr_proc = next_procs->p_first;
            next_procs = next_procs->p_rest;
        }
        else
            curr_proc = NULL;
    }
}

void
print_proc(FILE *fp, int indent, Proc proc) {
    /* fprintf(fp, "%*s ", indent, "proc"); */
    fprintf(fp, "%s ", "proc"); 
    print_header(fp, indent, proc->p_header);
    if(proc->p_decls != NULL)
        print_decls(fp, proc->p_decls);
    print_statements(fp, indent, proc->p_body);
}

void 
print_header(FILE *fp, int indent, Header header) {

    char    *h_id;
    Param    curr_param;
    Params   next_params;
    
    h_id = header->h_id;
    fprintf(fp, "%s(", h_id);
   
    if (header->h_params != NULL) {
        curr_param = header->h_params->p_first;
        next_params = header->h_params->p_rest;

        while(curr_param != NULL) {
            print_param(fp, curr_param);
            if (next_params != NULL) {
                curr_param = next_params->p_first;
                next_params = next_params->p_rest;
                fprintf(fp, ", ");
            }
            else
                curr_param = NULL;
        }
    }

    fprintf(fp, ")\n");
}

void 
print_param(FILE *fp, Param param) {

    switch (param->d_kind) {
        case VAL:
            fprintf(fp, "%s ", "val");
            break;
        case VALRES:
            fprintf(fp, "%s ", "valres");
            break;
        case REF:
            fprintf(fp, "%s ", "ref");
            break;
    }

    print_type(fp, param->d_type);

    fprintf(fp, "%s", param->d_id);
}

void
print_decls(FILE *fp, Decls decls) {

    Decl    curr_decl;
    Decls   next_decls;
    
    curr_decl = decls->d_first;
    next_decls = decls->d_rest;

    while(curr_decl != NULL) {
        /* fprintf(fp, "%s", curr_decl->); */
        print_type(fp, curr_decl->d_type);
        print_varnames(fp, curr_decl->d_varnames);
        fprintf(fp, ";\n");
        if (next_decls != NULL) {
            curr_decl = next_decls->d_first;
            next_decls = next_decls->d_rest;
        }
        else
            curr_decl = NULL;
    }
}

void
print_varnames(FILE *fp, VarNames varnames) {

    fprintf(fp, "%s", varnames->v_first->v_id);
        
    if (varnames->v_rest != NULL) {
        fprintf(fp, ", ");
        print_varnames(fp, varnames->v_rest);
    }
}

void
print_type(FILE *fp, VType type) {
    switch (type) {
        case INT:
            fprintf(fp, "%s ", "int");
            break;
        case FLOAT:
            fprintf(fp, "%s ", "float");
            break;
    }
}

void
print_statements(FILE *fp, int indent, Stmts stmts) {

    /* fprintf(fp, "%s", stmts->s_first->v_id); */
        
    if (stmts->s_rest != NULL) {
        fprintf(fp, "{\n");
        /* print_varnames(fp, varnames->v_rest); */
        fprintf(fp, "}\n");
    }
    else {
        fprintf(fp, "stmts->s_rest is NULL\n");
    }
}