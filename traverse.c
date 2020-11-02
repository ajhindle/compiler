#include <stdio.h>
#include "ast.h"
#include "pretty.h"

extern void report_error_and_exit(const char *msg);

const char *binopname[] = {BINOP_NAMES};
const char *unopname[] = {UNOP_NAMES};

//void proc_procs(FILE *fp, void (*f)(FILE *, Proc), Procs procs);
//void proc_header(FILE *fp, void (*f)(FILE *, Header), Header header);
//void proc_params(FILE *fp, Params params);
//void proc_decls(FILE *fp, Decls decls);
//void proc_type(FILE *fp, VType type);
//void proc_varnames(FILE *fp, VarNames varnames);
//void proc_statements(FILE *fp, Stmts stmts);
//void proc_statement(FILE *fp, Stmt stmt);
void proc_expressions(FILE *fp, Exprs exprs);
void proc_expression(FILE *fp, Expr expr);


void
proc_prog(FILE *fp, void (*f)(FILE *, Program), Program prog) {

    /* report_error_and_exit("Unable to pretty-proc"); */

    f(fp, prog);
    //f(fp, p_first);
    //proc_procs(fp, f, prog->procs);
    //proc_procs(fp, print_proc, prog->procs);
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
    // fprintf(fp, "%s(", header->h_id);
   
    //if (header->h_params != NULL) 
    //    proc_params(fp, header->h_params);

    //fprintf(fp, ")\n");
}

void 
proc_params(FILE *fp, void (*f)(FILE *, Params), Params params) {

    f(fp, params);

 /*   if (params->p_rest != NULL) {
        fprintf(fp, ", ");
        proc_params(fp, params->p_rest);
    }
    */
}

void
proc_decls(FILE *fp, void (*f)(FILE *, Decl), Decls decls) {

    f(fp, decls->d_first);

    if (decls->d_rest != NULL) 
        proc_decls(fp, f, decls->d_rest); 
}

void
proc_varnames(FILE *fp, void (*f)(FILE *, VarNames), VarNames varnames) {

    f(fp, varnames);
        
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
proc_expressions(FILE *fp, Exprs exprs) {
    /* only used in Calls ? */
    proc_expression(fp, exprs->e_first);
        
    if (exprs->e_rest != NULL) {
        fprintf(fp, ", ");
        proc_expressions(fp, exprs->e_rest);
    }
}

void
proc_expression(FILE *fp, Expr expr) {

    EKind e_kind = expr->e_kind;

    switch (e_kind) {
        case EXPR_ID:
            fprintf(fp, "%s", expr->e_id);
            break;
        case EXPR_INTCONST:
            fprintf(fp, "%d", expr->e_intval);
            break;
        case EXPR_FLTCONST:
            fprintf(fp, "%.2f", expr->e_fltval);
            break;
        case EXPR_BINOP:
            if (expr->e1->e_kind != EXPR_BINOP && 
                    expr->e1->e_kind != EXPR_UNOP && 
                    expr->e2->e_kind != EXPR_BINOP && 
                    expr->e2->e_kind != EXPR_UNOP ) 
            {
                fprintf(fp, "%s", "(");
                proc_expression(fp, expr->e1);
                fprintf(fp, " %s ", binopname[expr->e_binop]);
                proc_expression(fp, expr->e2);
                fprintf(fp, "%s", ")");
            }
            else {
                proc_expression(fp, expr->e1);
                fprintf(fp, " %s ", binopname[expr->e_binop]);
                proc_expression(fp, expr->e2);
            }

            break;
        case EXPR_UNOP:
            fprintf(fp, " %s", unopname[expr->e_unop]);
            proc_expression(fp, expr->e1);
            break;
        default: 
            fprintf(fp, "%s ", "UNKNOWN");
            break;
    }
}

