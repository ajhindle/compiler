#include <stdio.h>
#include <stdlib.h>
#include "n7c.h"
#include "ast.h"
#include "util.h"
#include "symbol.h"

Instr
alloc_code(int num_args) {

    Instr code;
    code = allocate(sizeof(struct s_instr));
    code->arg1 = allocate(sizeof(struct s_arg));
    code->arg2 = allocate(sizeof(struct s_arg));
    if (num_args > 2)
        code->arg3 = allocate(sizeof(struct s_arg));

    return code;
}
/*
void *
allocate(int size) {

    void    *addr;

    addr = malloc((size_t) size);
    if (addr == NULL) 
        report_error_and_exit("Out of memory");
    return addr;
}
*/
