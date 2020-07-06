#include <stdio.h>
#include "ast.h"

extern void report_error_and_exit(const char *msg);

void
pretty_prog(FILE *fp, Program prog) {

    /* report_error_and_exit("Unable to pretty-print"); */


    Procs   next_procs;
    Proc    curr_proc;

    curr_proc = prog->procs->p_first;
    next_procs = prog->procs->p_rest;

    while(curr_proc != NULL) {
        fprintf(stdout, "%s\n", "proc");
        curr_proc = next_procs->p_first;
        next_procs = next_procs->p_rest;

    }
}

