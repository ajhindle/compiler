#include <stdio.h>
#include "std.h"
#include "ast.h"

void proc_prog(FILE *fp, Program prog);
void proc_header(FILE *fp, void (*f)(FILE *, Header), Header header);
