#include <stdio.h>
#include "ast.h"
#include "traverse.h"

extern void report_error_and_exit(const char *msg);

const int INDENT = 4;
const int INDENT_START = 0;
const char *binopname[] = {BINOP_NAMES};
const char *unopname[] = {UNOP_NAMES};

void print_procs(FILE *fp, int indent, Procs procs);
void print_header(FILE *fp, int indent, Header heaccder);
void print_params(FILE *fp, Params params);
void print_decl(FILE *fp, Decl decls);
void print_type(FILE *fp, VType type);
void print_varnames(FILE *fp, VarNames varnames);
void print_statements(FILE *fp, Stmts stmts);
void print_statement(FILE *fp, Stmt stmt);
void print_expressions(FILE *fp, Exprs exprs);
void print_expression(FILE *fp, Expr expr);

void print_hdr(FILE *fp, Header header);

void
print_proc(FILE *fp, Proc proc) {
    fprintf(fp, "%s ", "proc"); 
    proc_header(fp, print_hdr, proc->p_header);

    if(proc->p_decls != NULL)
        proc_decls(fp, print_decl, proc->p_decls);
    
    proc_statements(fp, print_statements, proc->p_body);
    fprintf(fp, "\nend\n\n"); 

    return;
}

void
print_hdr(FILE *fp, Header header) {
    
    fprintf(fp, "%s(", header->h_id);
    
    if (header->h_params != NULL) 
        proc_params(fp, print_params, header->h_params);

    fprintf(fp, ")\n");

    return;
}


void
pretty_prog(FILE *fp, Program prog) {

    /* report_error_and_exit("Unable to pretty-print"); */

    //int indent = INDENT_START;
    //print_procs(fp, indent, prog->procs);
    proc_procs(fp, print_proc, prog->procs);
}

/* DELETE / replace this function */
void
print_procs(FILE *fp, int indent, Procs procs) {

    fprintf(fp, "%*s ", indent, "proc"); 
    print_header(fp, indent, procs->p_first->p_header);

    //if(procs->p_first->p_decls != NULL)
    //    print_decls(fp, procs->p_first->p_decls);
    
    //print_statements(fp, indent, procs->p_first->p_body);
    fprintf(fp, "%*s", indent, "\nend\n\n"); 
    
    if(procs->p_rest != NULL)
        print_procs(fp, indent, procs->p_rest);
}

void 
print_header(FILE *fp, int indent, Header header) {

    fprintf(fp, "%s(", header->h_id);
   
    if (header->h_params != NULL) 
        print_params(fp, header->h_params);

    fprintf(fp, ")\n");
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
    proc_varnames(fp, print_varnames, decl->d_varnames);
    fprintf(fp, ";\n");
}

void
print_varnames(FILE *fp, VarNames varnames) {

    fprintf(fp, "%s", varnames->v_first->v_id);
        
    if (varnames->v_rest != NULL) {
        fprintf(fp, ", ");
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

    if (stmts->s_rest != NULL) {
        fprintf(fp, ";\n");
        //proc_statement(fp, print_statement, stmts->s_rest); 
    }
}

void
print_statement(FILE *fp, Stmt stmt) {

    SKind s_kind = stmt->s_kind;

    switch (s_kind) {
        case STMT_ASSIGN:
            fprintf(fp, "%s := ", stmt->s_info.s_assign.asg_id);
            proc_expression(fp, print_expression, stmt->s_info.s_assign.asg_expr);
            break;
        case STMT_BLOCK:
            fprintf(fp, "{\n");
            proc_statements(fp, print_statements, stmt->s_info.s_block);
            fprintf(fp, "\n}");
            break;
        case STMT_COND:
            fprintf(fp, "%s ", "if");
            proc_expression(fp, print_expression, stmt->s_info.s_cond.if_cond);
            fprintf(fp, " %s ", "then");
            proc_statement(fp, print_statement, stmt->s_info.s_cond.if_then);
            fprintf(fp, "%s ", "\nelse");
            proc_statement(fp, print_statement, stmt->s_info.s_cond.if_else);
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
            proc_expression(fp, print_expression, stmt->s_info.s_while.while_cond);
            fprintf(fp, " %s ", "do");
            proc_statement(fp, print_statement, stmt->s_info.s_while.while_body);
            break;
        case STMT_WRITE:
            fprintf(fp, "%s ", "write");
            proc_expression(fp, print_expression, stmt->s_info.s_write);
            break;
        case STMT_FOR:
            fprintf(fp, "%s ", "for");
            fprintf(fp, "%s ", stmt->s_info.s_for.for_id);
            proc_expression(fp, print_expression, stmt->s_info.s_for.for_from_expr);
            fprintf(fp, " %s ", "do");
            proc_expression(fp, print_expression, stmt->s_info.s_for.for_to_expr);
            proc_statement(fp, print_statement, stmt->s_info.s_for.for_body);
            break;
        case STMT_CALL:
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
            fprintf(fp, " %s", unopname[expr->e_unop]);
            proc_expression(fp, print_expression, expr->e1);
            break;
        default: 
            fprintf(fp, "%s ", "UNKNOWN");
            break;
    }
}

