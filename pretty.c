/*
 * Author   : ajhindle
 * Date     : 25-11-2020
 *
 * File     : pretty.c
 *
 * This module provides procedures that traverse the AST and pretty prints 
 * the program.
*/

#include <stdio.h>
#include "ast.h"
#include "traverse.h"
#include "util.h"


const char *binopname[] = {BINOP_NAMES};
const char *unopname[] = {UNOP_NAMES};

void print_proc(FILE *fp, Proc proc);
void print_header(FILE *fp, Header header);
void print_params(FILE *fp, Params params);
void print_decl(FILE *fp, Decl decls);
void print_type(FILE *fp, VType type);
void print_varnames(FILE *fp, VarNames varnames);
void print_statements(FILE *fp, Stmts stmts);
void print_statement(FILE *fp, Stmt stmt);
void print_expressions(FILE *fp, Exprs exprs);
void print_expression(FILE *fp, Expr expr);
void indent_here(FILE *fp);
void block_handler(FILE *fp, Stmt stmt);

// Number of spaces to indent with
const static int indent = 4;

// Point on line (column) to start writing
static int write_pt = 0;

static BOOL brace_on_newline = TRUE;

void
pretty_prog(FILE *fp, Program prog) {

    /* report_error_and_exit("Unable to pretty-print"); */

    proc_procs(fp, print_proc, prog->procs);
}


void
print_proc(FILE *fp, Proc proc) {
    
    write_pt = 0;
    indent_here(fp);
    fprintf(fp, "%s ", "proc"); 
    proc_header(fp, print_header, proc->p_header);

    if(proc->p_decls != NULL)
        proc_decls(fp, print_decl, proc->p_decls);

    proc_statements(fp, print_statements, proc->p_body);

    fprintf(fp, "\n"); 
    indent_here(fp);
    fprintf(fp, "%s \n\n", "end"); 

    return;
}


void
print_header(FILE *fp, Header header) {
    
    fprintf(fp, "%s(", header->h_id);
    
    if (header->h_params != NULL) 
        proc_params(fp, print_params, header->h_params);

    fprintf(fp, ")\n");

    return;
}


void 
print_params(FILE *fp, Params params) {

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

    print_type(fp, params->p_first->d_type);
    fprintf(fp, "%s", params->p_first->d_id);

    if (params->p_rest != NULL) {
        fprintf(fp, ", ");
        print_params(fp, params->p_rest);
    }
}


void
print_decl(FILE *fp, Decl decl) {

    proc_type(fp, print_type, decl->d_type);
    print_varnames(fp, decl->d_varnames);
    fprintf(fp, ";\n");
}


void
print_varnames(FILE *fp, VarNames varnames) {

    fprintf(fp, "%s", varnames->v_first->v_id);
        
    if (varnames->v_rest != NULL) {
        fprintf(fp, ", ");
        print_varnames(fp, varnames->v_rest);
    }
}


void
print_type(FILE *fp, VType type) {
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
print_statements(FILE *fp, Stmts stmts) {

    if (stmts->s_first != NULL) {
        proc_statement(fp, print_statement, stmts->s_first); 
    }

    if (stmts->s_rest != NULL) 
        fprintf(fp, ";\n");
}


/*
 * Writes spaces to indent lines of the pretty-printed program.
 */
void
indent_here(FILE *fp) {
    int i;
    //fprintf(fp, "%*s", write_pt, " ");
    //TODO use conditional debugging to turn this line on/off
    fprintf(fp, "%d", write_pt);
    for (i = 0; i < write_pt; i++)
        putc((int)' ', fp);
}


/* 
 * This procedure is used for body statements of while-loops, for-loops and 
 * if-then-else.  If the body is a block of statements, the brace must appear 
 * on the same line as e.g. the 'else' keyword.  If body is not a block then 
 * just indent it.
 */
void
block_handler(FILE *fp, Stmt stmt) {

    if (stmt->s_kind == STMT_BLOCK) {
        fprintf(fp, "{\n");
        brace_on_newline = FALSE;
        proc_statement(fp, print_statement, stmt);
        brace_on_newline = TRUE;
    }
    else {
        fprintf(fp, "\n");
        write_pt += indent;
        proc_statement(fp, print_statement, stmt);
        write_pt -= indent;
    }
}


void
print_statement(FILE *fp, Stmt stmt) {

    SKind   s_kind = stmt->s_kind;

    switch (s_kind) {
        case STMT_ASSIGN:
            indent_here(fp);
            fprintf(fp, "%s := ", stmt->s_info.s_assign.asg_id);
            proc_expression(fp, print_expression, stmt->s_info.s_assign.asg_expr);
            break;
        case STMT_BLOCK:
            if (brace_on_newline == TRUE) {
                indent_here(fp);
                fprintf(fp, "{\n");
            }
            write_pt += indent;
            proc_statements(fp, print_statements, stmt->s_info.s_block);
            write_pt -= indent;
            fprintf(fp, "\n");
            indent_here(fp);
            fprintf(fp, "}");
            break;
        case STMT_COND:
            indent_here(fp);
            fprintf(fp, "%s ", "if");
            proc_expression(fp, print_expression, stmt->s_info.s_cond.if_cond);
            fprintf(fp, " %s ", "then");
            /*
            fprintf(fp, "%s", "\n");
            write_pt += indent;
            proc_statement(fp, print_statement, stmt->s_info.s_cond.if_then);
            write_pt -= indent;
            */
            block_handler(fp, stmt->s_info.s_cond.if_then);
            fprintf(fp, "%s", "\n");
            indent_here(fp);
            fprintf(fp, "%s", "else ");
            block_handler(fp, stmt->s_info.s_cond.if_else);
            /*
            fprintf(fp, "%s", "\n");
            write_pt += indent;
            proc_statement(fp, print_statement, stmt->s_info.s_cond.if_else);
            write_pt -= indent;
            */
            break;
        case STMT_READ:
            indent_here(fp);
            fprintf(fp, "%s ", "read");
            fprintf(fp, "%s", stmt->s_info.s_read);
            break;
        case STMT_SKIP:
            indent_here(fp);
            fprintf(fp, "%s", "skip");
            break;
        case STMT_WHILE:
            indent_here(fp);
            fprintf(fp, "%s ", "while");
            proc_expression(fp, print_expression, stmt->s_info.s_while.while_cond);
            fprintf(fp, " %s ", "do");
            block_handler(fp, stmt->s_info.s_while.while_body);
            /*
            fprintf(fp, " %s", "\n");
            write_pt += indent;
            proc_statement(fp, print_statement, stmt->s_info.s_while.while_body);
            write_pt -= indent;
            */
            break;
        case STMT_WRITE:
            indent_here(fp);
            fprintf(fp, "%s ", "write");
            proc_expression(fp, print_expression, stmt->s_info.s_write);
            break;
        case STMT_FOR:
            indent_here(fp);
            fprintf(fp, "%s ", "for");
            fprintf(fp, "%s ", stmt->s_info.s_for.for_id);
            proc_expression(fp, print_expression, stmt->s_info.s_for.for_from_expr);
            fprintf(fp, " %s ", "do");
            proc_expression(fp, print_expression, stmt->s_info.s_for.for_to_expr);
            proc_statement(fp, print_statement, stmt->s_info.s_for.for_body);
            break;
        case STMT_CALL:
            indent_here(fp);
            fprintf(fp, "%s(", stmt->s_info.s_call.call_id);
            proc_expressions(fp, print_expressions, stmt->s_info.s_call.s_exprs);
            fprintf(fp, ")");
            break;
    }
}


void
print_expressions(FILE *fp, Exprs exprs) {
    /* only used in Calls ? */
    print_expression(fp, exprs->e_first);
        
    if (exprs->e_rest != NULL) {
        fprintf(fp, ", ");
        print_expressions(fp, exprs->e_rest);
    }
}


void
print_expression(FILE *fp, Expr expr) {

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
        case EXPR_STRCONST:
            fprintf(fp, "%s", expr->e_id);
            break;
        case EXPR_BINOP:
            if (expr->e1->e_kind != EXPR_BINOP && 
                    expr->e1->e_kind != EXPR_UNOP && 
                    expr->e2->e_kind != EXPR_BINOP && 
                    expr->e2->e_kind != EXPR_UNOP ) 
            {
                fprintf(fp, "%s", "(");
                proc_expression(fp, print_expression, expr->e1);
                fprintf(fp, " %s ", binopname[expr->e_binop]);
                proc_expression(fp, print_expression, expr->e2);
                fprintf(fp, "%s", ")");
            }
            else {
                proc_expression(fp, print_expression, expr->e1);
                fprintf(fp, " %s ", binopname[expr->e_binop]);
                proc_expression(fp, print_expression, expr->e2);
            }
            break;
        case EXPR_UNOP:
            fprintf(fp, "%s ", unopname[expr->e_unop]);
            proc_expression(fp, print_expression, expr->e1);
            break;
        default: 
            fprintf(fp, "Unknown expression type at line %d", expr->e_lineno);
            report_error_and_exit("Unable to pretty print");
            break;
    }
}

