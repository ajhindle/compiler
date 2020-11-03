#include <stdio.h>
#include "std.h"
#include "ast.h"

//void proc_prog(FILE *fp, Program prog);
void proc_prog(FILE *fp, void (*f)(FILE *, Program), Program prog);
void proc_procs(FILE *fp, void (*f)(FILE *, Proc), Procs procs);
void proc_header(FILE *fp, void (*f)(FILE *, Header), Header header);
void proc_params(FILE *fp, void (*f)(FILE *, Params), Params params);
void proc_decls(FILE *fp, void (*f)(FILE *, Decl), Decls decls);
void proc_varnames(FILE *fp, void (*f)(FILE *, VarNames), VarNames varnames);
void proc_type(FILE *fp, void (*f)(FILE *, VType), VType type);
void proc_statements(FILE *fp, void (*f)(FILE *, Stmts), Stmts stmts);
void proc_statement(FILE *fp, void (*f)(FILE *, Stmt), Stmt stmt); 
void proc_expressions(FILE *fp, void (*f)(FILE *, Exprs), Exprs exprs);
