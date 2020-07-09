#include <stdio.h>
#include "ast.h"

extern void report_error_and_exit(const char *msg);

void print_proc(Proc proc);
void print_header(Header header);
void print_param(Param param);

void
pretty_prog(FILE *fp, Program prog) {

    /* report_error_and_exit("Unable to pretty-print"); */

    Procs   next_procs;
    Proc    curr_proc;

    curr_proc = prog->procs->p_first;
    next_procs = prog->procs->p_rest;

    while(curr_proc != NULL) {
        print_proc(curr_proc);
        if (next_procs != NULL) {
            curr_proc = next_procs->p_first;
            next_procs = next_procs->p_rest;
        }
        else
            curr_proc = NULL;
    }
}

void
print_proc(Proc proc) {
    fprintf(stdout, "%s ", "proc");
    print_header(proc->p_header);
}

void 
print_header(Header header) {

    char    *h_id;
    Param    curr_param;
    Params   next_params;
    
    h_id = header->h_id;
    fprintf(stdout, "%s(", h_id);
   
    if (header->h_params != NULL) {
        curr_param = header->h_params->p_first;
        next_params = header->h_params->p_rest;

        while(curr_param != NULL) {
            print_param(curr_param);
            if (next_params != NULL) {
                curr_param = next_params->p_first;
                next_params = next_params->p_rest;
            }
            else
            curr_param = NULL;
        }
    }

    fprintf(stdout, ")\n");
}

void 
print_param(Param param) {
    fprintf(stdout, "%s", param->d_id);
}
