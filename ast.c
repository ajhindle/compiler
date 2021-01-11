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
    code->num_args = num_args;
    code->arg1 = allocate(sizeof(struct s_arg));
    code->arg2 = allocate(sizeof(struct s_arg));
    code->arg3 = allocate(sizeof(struct s_arg));

    return code;
}

Arg
alloc_instr_arg(void) {

    Arg arg;

    arg = allocate(sizeof(struct s_arg));

    return arg;
}
