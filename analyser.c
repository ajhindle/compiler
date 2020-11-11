#include <stdio.h>
#include "ast.h"
#include "traverse.h"

extern void report_error_and_exit(const char *msg);


void analyse_proc(FILE *fp, Proc proc);
void analyse_header(FILE *fp, Header header);
void analyse_params(FILE *fp, Params params);
void analyse_decls(FILE *fp, Decls decls);
void analyse_type(FILE *fp, VType type);
void analyse_varnames(FILE *fp, VarNames varnames);
void analyse_statements(FILE *fp, Stmts stmts);
void analyse_statement(FILE *fp, Stmt stmt);
void analyse_expressions(FILE *fp, Exprs exprs);
void analyse_expression(FILE *fp, Expr expr);

int param_ct;
int var_ct;

void
analyse_prog(FILE *fp, Program prog) {

    /* report_error_and_exit("Unable to pretty-print"); */

    proc_procs(fp, analyse_proc, prog->procs);
}

void
analyse_proc(FILE *fp, Proc proc) {

    param_ct = 0;
    var_ct = 0;

    proc_header(fp, analyse_header, proc->p_header);
    
    proc->p_param_ct = param_ct;
    proc->p_var_ct = var_ct;

    if(proc->p_decls != NULL) {
        analyse_decls(fp, proc->p_decls);
        proc->p_var_ct = var_ct;
    }
    
    analyse_statements(fp, proc->p_body);
}

void 
analyse_header(FILE *fp, Header header) {
   
    if (header->h_params != NULL) 
        analyse_params(fp, header->h_params);
}

void 
analyse_params(FILE *fp, Params params) {

    param_ct = param_ct + 1;

    switch (params->p_first->d_kind) {
        case VAL:
            break;
        case VALRES:
            break;
        case REF:
            break;
    }

    analyse_type(fp, params->p_first->d_type);

    if (params->p_rest != NULL) {
        analyse_params(fp, params->p_rest);
    }
}

void
analyse_decls(FILE *fp, Decls decls) {

    var_ct = var_ct + 1;
    analyse_type(fp, decls->d_first->d_type);
    analyse_varnames(fp, decls->d_first->d_varnames);
    if (decls->d_rest != NULL) 
        analyse_decls(fp, decls->d_rest); 
}

void
analyse_varnames(FILE *fp, VarNames varnames) {

    if (varnames->v_rest != NULL) {
        analyse_varnames(fp, varnames->v_rest);
    }
}

void
analyse_type(FILE *fp, VType type) {
    switch (type) {
        case INT:
            break;
        case FLOAT:
            break;
    }
}

void
analyse_statements(FILE *fp, Stmts stmts) {

    if (stmts->s_first != NULL) {
        analyse_statement(fp, stmts->s_first); 
    }

    if (stmts->s_rest != NULL) {
        analyse_statements(fp, stmts->s_rest); 
    }
}

void
analyse_statement(FILE *fp, Stmt stmt) {

    SKind s_kind = stmt->s_kind;

    switch (s_kind) {
        case STMT_ASSIGN:
            analyse_expression(fp, stmt->s_info.s_assign.asg_expr);
            break;
        case STMT_BLOCK:
            analyse_statements(fp, stmt->s_info.s_block);
            break;
        case STMT_COND:
            analyse_expression(fp, stmt->s_info.s_cond.if_cond);
            analyse_statement(fp, stmt->s_info.s_cond.if_then);
            analyse_statement(fp, stmt->s_info.s_cond.if_else);
            break;
        case STMT_READ:
            break;
        case STMT_SKIP:
            break;
        case STMT_WHILE:
            analyse_expression(fp, stmt->s_info.s_while.while_cond);
            analyse_statement(fp, stmt->s_info.s_while.while_body);
            break;
        case STMT_WRITE:
            analyse_expression(fp, stmt->s_info.s_write);
            break;
        case STMT_FOR:
            analyse_expression(fp, stmt->s_info.s_for.for_from_expr);
            analyse_expression(fp, stmt->s_info.s_for.for_to_expr);
            analyse_statement(fp, stmt->s_info.s_for.for_body);
            break;
        case STMT_CALL:
            analyse_expressions(fp, stmt->s_info.s_call.s_exprs);
            break;
    }
}

void
analyse_expressions(FILE *fp, Exprs exprs) {
    /* only used in Calls ? */
    analyse_expression(fp, exprs->e_first);
        
    if (exprs->e_rest != NULL) {
        analyse_expressions(fp, exprs->e_rest);
    }
}

void
analyse_expression(FILE *fp, Expr expr) {

    EKind e_kind = expr->e_kind;

    switch (e_kind) {
        case EXPR_ID:
            /* TODO look up the symbol table to find the type */
            expr->e_type = INT;
            fprintf(fp, "Value %d is type INT\n", expr->e_intval);
            break;
        case EXPR_INTCONST:
            expr->e_type = INT;
            fprintf(fp, "Value %d is type INT\n", expr->e_intval);
            break;
        case EXPR_FLTCONST:
            expr->e_type = FLOAT;
            fprintf(fp, "Value %.3f is type FLOAT\n", expr->e_fltval);
            break;
        case EXPR_BINOP:
            analyse_expression(fp, expr->e1);
            analyse_expression(fp, expr->e2);
            if (expr->e1->e_type == INT && expr->e2->e_type == INT) {
                expr->e_type = INT;
                fprintf(fp, "Binop is type INT\n");
                return;
            }
            if (expr->e1->e_type == FLOAT && expr->e2->e_type == FLOAT) {
                expr->e_type = FLOAT;
                fprintf(fp, "Binop is type FLOAT\n");
                return;
            }
            if (expr->e1->e_type == INT && expr->e2->e_type == FLOAT) {
                expr->e_type = FLOAT;
                fprintf(fp, "Binop is type FLOAT\n");
                return;
            }
            if (expr->e1->e_type == FLOAT && expr->e2->e_type == INT) {
                expr->e_type = FLOAT;
                fprintf(fp, "Binop is type FLOAT\n");
                return;
            }
            break;
        case EXPR_UNOP:
            analyse_expression(fp, expr->e1);
            break;
        default: 
            break;
    }
}

