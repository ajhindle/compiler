/*
 * Author   : ajhindle
 * Date     : 25-11-2020
 *
 * File     : traverse.c
 *
 * This module provides 'higher order' procedures that traverse the AST.  
*/

#include <stdio.h>
#include "ast.h"

extern void report_error_and_exit(const char *msg);

void
proc_prog(FILE *fp, void (*f)(FILE *, Program), Program prog) {

    f(fp, prog);
}


void
proc_procs(FILE *fp, void (*f)(FILE *, Proc), Procs procs) {

    Proc p_first = procs->p_first;
    Procs p_rest = procs->p_rest;

    f(fp, p_first);
    
    if(p_rest != NULL)
        proc_procs(fp, *f, p_rest);
}

void 
proc_header(FILE *fp, void (*f)(FILE *, Header), Header header) {

    f(fp, header);
}

void 
proc_params(FILE *fp, void (*f)(FILE *, Params), Params params) {

    f(fp, params);
}

void
proc_decls(FILE *fp, void (*f)(FILE *, Decl), Decls decls) {

    f(fp, decls->d_first);

    if (decls->d_rest != NULL) 
        proc_decls(fp, f, decls->d_rest); 
}

void
proc_varnames(FILE *fp, void (*f)(FILE *, VarName), VarNames varnames) {

    f(fp, varnames->v_first);
        
    if (varnames->v_rest != NULL) 
        proc_varnames(fp, f, varnames->v_rest);
}

void
proc_type(FILE *fp, void (*f)(FILE *, VType), VType type) {
    
    f(fp, type);
}

void
proc_statements(FILE *fp, void (*f)(FILE *, Stmts), Stmts stmts) {

    f(fp, stmts);

    if (stmts->s_rest != NULL) 
        proc_statements(fp, f, stmts->s_rest); 
}

void
proc_statement(FILE *fp, void (*f)(FILE *, Stmt), Stmt stmt) {

    f(fp, stmt);
}

void
proc_expressions(FILE *fp, void (*f)(FILE *, Exprs), Exprs exprs) {
    /* only used in Calls ? */
    f(fp, exprs);
        
    if (exprs->e_rest != NULL) {
        proc_expressions(fp, f, exprs->e_rest);
    }
}

void
proc_expression(FILE *fp, void (*f)(FILE *, Expr), Expr expr) {

    f(fp, expr);

}

