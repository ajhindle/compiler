#include <stdio.h>
#include "std.h"
#include "ast.h"

//void proc_prog(FILE *fp, Program prog);
void proc_prog(FILE *fp, void (*f)(FILE *, Program), Program prog);
void proc_procs(FILE *fp, void (*f)(FILE *, Proc), Procs procs);
void proc_header(FILE *fp, void (*f)(FILE *, Header), Header header);
void proc_params(FILE *fp, void (*f)(FILE *, Params), Params params);
