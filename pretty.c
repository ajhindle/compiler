#include <stdio.h>
#include "ast.h"

extern void report_error_and_exit(const char *msg);

const int INDENT = 4;
const int INDENT_START = 0;
const char *binopname[] = {BINOP_NAMES};
const char *unopname[] = {UNOP_NAMES};

void proc_procs(FILE *fp, void(*f)(FILE *), Procs procs);
void proc_header(FILE *fp, Header header);
void proc_params(FILE *fp, Params params);
void proc_decls(FILE *fp, Decls decls);
void proc_type(FILE *fp, VType type);
void proc_varnames(FILE *fp, VarNames varnames);
void proc_statements(FILE *fp, Stmts stmts);
void proc_statement(FILE *fp, Stmt stmt);
void proc_expressions(FILE *fp, Exprs exprs);
void proc_expression(FILE *fp, Expr expr);
void demo_print(FILE *fp);

void
proc_prog(FILE *fp, Program prog) {

    /* report_error_and_exit("Unable to pretty-proc"); */

    int indent = INDENT_START;
    proc_procs(fp, demo_print, prog->procs);
}

void
demo_print(FILE *fp) {
    fprintf(fp, "%s ", "... from higher-order:\n"); 
    fprintf(fp, "%s ", "proc"); 
    return;
}

void
proc_procs(FILE *fp, void (*f)(FILE *), Procs procs) {

    Proc p_first = procs->p_first;
    Procs p_rest = procs->p_rest;

    //fprintf(fp, "%s ", "proc"); 
    f(fp);
    proc_header(fp, p_first->p_header);

    if(p_first->p_decls != NULL)
        proc_decls(fp, p_first->p_decls);
    
//    proc_statements(fp, p_first->p_body);
//    fprintf(fp, "%*s", "\nend\n\n"); 
    
    if(p_rest != NULL)
        proc_procs(fp, *f, p_rest);
}

void 
proc_header(FILE *fp, Header header) {

    fprintf(fp, "%s(", header->h_id);
   
    if (header->h_params != NULL) 
        proc_params(fp, header->h_params);

    fprintf(fp, ")\n");
}

void 
proc_params(FILE *fp, Params params) {

    switch (params->p_first->d_kind) {
        case VAL:
            fprintf(fp, "%s ", "val");
            break;
        case VALRES:
            fprintf(fp, "%s ", "valres");
            break;
        case REF:
            fprintf(fp, "%s ", "ref");
            break;
    }

    proc_type(fp, params->p_first->d_type);
    fprintf(fp, "%s", params->p_first->d_id);

    if (params->p_rest != NULL) {
        fprintf(fp, ", ");
        proc_params(fp, params->p_rest);
    }
}

void
proc_decls(FILE *fp, Decls decls) {

    proc_type(fp, decls->d_first->d_type);
    proc_varnames(fp, decls->d_first->d_varnames);
    fprintf(fp, ";\n");
    if (decls->d_rest != NULL) 
        proc_decls(fp, decls->d_rest); 
}

void
proc_varnames(FILE *fp, VarNames varnames) {

    fprintf(fp, "%s", varnames->v_first->v_id);
        
    if (varnames->v_rest != NULL) {
        fprintf(fp, ", ");
        proc_varnames(fp, varnames->v_rest);
    }
}

void
proc_type(FILE *fp, VType type) {
    switch (type) {
        case INT:
            fprintf(fp, "%s ", "int");
            break;
        case FLOAT:
            fprintf(fp, "%s ", "float");
            break;
    }
}

void
proc_statements(FILE *fp, Stmts stmts) {

    if (stmts->s_first != NULL) {
        proc_statement(fp, stmts->s_first); 
    }

    if (stmts->s_rest != NULL) {
        fprintf(fp, ";\n");
        proc_statements(fp, stmts->s_rest); 
    }
}

void
proc_statement(FILE *fp, Stmt stmt) {

    SKind s_kind = stmt->s_kind;

    switch (s_kind) {
        case STMT_ASSIGN:
            fprintf(fp, "%s := ", stmt->s_info.s_assign.asg_id);
            proc_expression(fp, stmt->s_info.s_assign.asg_expr);
            break;
        case STMT_BLOCK:
            fprintf(fp, "{\n");
            proc_statements(fp, stmt->s_info.s_block);
            fprintf(fp, "\n}");
            break;
        case STMT_COND:
            fprintf(fp, "%s ", "if");
            proc_expression(fp, stmt->s_info.s_cond.if_cond);
            fprintf(fp, " %s ", "then");
            proc_statement(fp, stmt->s_info.s_cond.if_then);
            fprintf(fp, "%s ", "\nelse");
            proc_statement(fp, stmt->s_info.s_cond.if_else);
            break;
        case STMT_READ:
            fprintf(fp, "%s ", "read");
            fprintf(fp, "%s", stmt->s_info.s_read);
            break;
        case STMT_SKIP:
            fprintf(fp, "%s", "skip");
            break;
        case STMT_WHILE:
            fprintf(fp, "%s ", "while");
            proc_expression(fp, stmt->s_info.s_while.while_cond);
            fprintf(fp, " %s ", "do");
            proc_statement(fp, stmt->s_info.s_while.while_body);
            break;
        case STMT_WRITE:
            fprintf(fp, "%s ", "write");
            proc_expression(fp, stmt->s_info.s_write);
            break;
        case STMT_FOR:
            fprintf(fp, "%s ", "for");
            fprintf(fp, "%s ", stmt->s_info.s_for.for_id);
            proc_expression(fp, stmt->s_info.s_for.for_from_expr);
            fprintf(fp, " %s ", "do");
            proc_expression(fp, stmt->s_info.s_for.for_to_expr);
            proc_statement(fp, stmt->s_info.s_for.for_body);
            break;
        case STMT_CALL:
            fprintf(fp, "%s(", stmt->s_info.s_call.call_id);
            proc_expressions(fp, stmt->s_info.s_call.s_exprs);
            fprintf(fp, ")");
            break;
    }
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

