#include <stdio.h>
#include "ast.h"

extern void report_error_and_exit(const char *msg);

const int INDENT = 10;

void print_proc(FILE *fp, int indent, Proc proc);
void print_header(FILE *fp, int indent, Header header);
void print_param(FILE *fp, Param param);
void print_decls(FILE *fp, Decls decls);
void print_type(FILE *fp, VType type);
void print_varnames(FILE *fp, VarNames varnames);
void print_statements(FILE *fp, int indent, Stmts stmts);
void print_statement(FILE *fp, Stmt stmt);
void print_binop(FILE *fp, BinOp binop);
void print_expressions(FILE *fp, int ident, Exprs exprs);
void print_expression(FILE *fp, Expr expr);

void
pretty_prog(FILE *fp, Program prog) {

    /* report_error_and_exit("Unable to pretty-print"); */

    Procs   next_procs;
    Proc    curr_proc;

    curr_proc = prog->procs->p_first;
    next_procs = prog->procs->p_rest;

    while(curr_proc != NULL) {
        print_proc(fp, INDENT, curr_proc);
        if (next_procs != NULL) {
            curr_proc = next_procs->p_first;
            next_procs = next_procs->p_rest;
        }
        else
            curr_proc = NULL;
    }
}

void
print_proc(FILE *fp, int indent, Proc proc) {
    /* fprintf(fp, "%*s ", indent, "proc"); */
    fprintf(fp, "%s ", "proc"); 
    print_header(fp, indent, proc->p_header);
    if(proc->p_decls != NULL)
        print_decls(fp, proc->p_decls);
    print_statements(fp, indent, proc->p_body);
}

void 
print_header(FILE *fp, int indent, Header header) {

    char    *h_id;
    Param    curr_param;
    Params   next_params;
    
    h_id = header->h_id;
    fprintf(fp, "%s(", h_id);
   
    if (header->h_params != NULL) {
        curr_param = header->h_params->p_first;
        next_params = header->h_params->p_rest;

        while(curr_param != NULL) {
            print_param(fp, curr_param);
            if (next_params != NULL) {
                curr_param = next_params->p_first;
                next_params = next_params->p_rest;
                fprintf(fp, ", ");
            }
            else
                curr_param = NULL;
        }
    }

    fprintf(fp, ")\n");
}

void 
print_param(FILE *fp, Param param) {

    switch (param->d_kind) {
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

    print_type(fp, param->d_type);

    fprintf(fp, "%s", param->d_id);
}

void
print_decls(FILE *fp, Decls decls) {

    Decl    curr_decl;
    Decls   next_decls;
    
    curr_decl = decls->d_first;
    next_decls = decls->d_rest;

    while(curr_decl != NULL) {
        /* fprintf(fp, "%s", curr_decl->); */
        print_type(fp, curr_decl->d_type);
        print_varnames(fp, curr_decl->d_varnames);
        fprintf(fp, ";\n");
        if (next_decls != NULL) {
            curr_decl = next_decls->d_first;
            next_decls = next_decls->d_rest;
        }
        else
            curr_decl = NULL;
    }
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
print_statements(FILE *fp, int indent, Stmts stmts) {

        
    if (stmts->s_first != NULL) {
        print_statement(fp, stmts->s_first); 
        fprintf(fp, "\n");
    }

    if (stmts->s_rest != NULL) {
        print_statements(fp, indent, stmts->s_rest); 
        fprintf(fp, "\n");
    }
}

void
print_statement(FILE *fp, Stmt stmt) {

    SKind s_kind = stmt->s_kind;

    switch (s_kind) {
        case STMT_ASSIGN:
            fprintf(fp, "%s := ", stmt->s_info.s_assign.asg_id);
            print_expression(fp, stmt->s_info.s_assign.asg_expr);
            break;
        case STMT_BLOCK:
            fprintf(fp, "{\n");
            /* fprintf(fp, "%s ", "{ <block> }"); */
            print_statements(fp, 2, stmt->s_info.s_block);
            fprintf(fp, "}\n");
            break;
        case STMT_COND:
            fprintf(fp, "%s ", "if \n");
            fprintf(fp, "%s ", "then ");
            print_statement(fp, stmt->s_info.s_cond.if_then);
            fprintf(fp, "%s ", "else ");
            print_statement(fp, stmt->s_info.s_cond.if_else);
            break;
        case STMT_READ:
            fprintf(fp, "%s ", "read");
            break;
        case STMT_SKIP:
            fprintf(fp, "%s ", "skip");
            break;
        case STMT_WHILE:
            fprintf(fp, "%s ", "while ");
            print_statement(fp, stmt->s_info.s_while.while_body);
            break;
        case STMT_WRITE:
            fprintf(fp, "%s ", "write");
            break;
        case STMT_FOR:
            fprintf(fp, "%s ", "for");
            break;
        case STMT_CALL:
            fprintf(fp, "%s ", "call");
            break;
        default:
            fprintf(fp, "%s ", "UNKNOWN");
            break;
    }
}

void
print_expressions(FILE *fp, int indent, Exprs exprs) {

    print_expression(fp, exprs->e_first);
        
    if (exprs->e_rest != NULL) 
        print_expressions(fp, indent, exprs->e_rest);
    
}

void
print_expression(FILE *fp, Expr expr) {

    EKind e_kind = expr->e_kind;

    switch (e_kind) {
        case EXPR_ID:
            fprintf(fp, "%s ", expr->e_id);
            break;
        case EXPR_INTCONST:
            fprintf(fp, "%d ", expr->e_val);
            break;
        case EXPR_FLTCONST:
            fprintf(fp, "%.2f ", expr->e_float);
            break;
        case EXPR_BINOP:
            print_expression(fp, expr->e1);
            /* fprintf(fp, " %s ", binopname[expr->e_binop]);*/
            print_binop(fp, expr->e_binop);
            print_expression(fp, expr->e2);
            break;
        case EXPR_UNOP:
            fprintf(fp, "%s ", "expr_unop");
            break;
        default: 
            fprintf(fp, "%s ", "UNKNOWN");
            break;
    }
}


void
print_binop(FILE *fp, BinOp binop) {
    switch (binop) {
        case BINOP_ADD:
            fprintf(fp, "%s ", "+");
            break;
        case BINOP_SUB:
            fprintf(fp, "%s ", "-");
            break;
        case BINOP_MUL:
            fprintf(fp, "%s ", "*");
            break;
        case BINOP_DIV:
            fprintf(fp, "%s ", "/");
            break;
        case BINOP_EQ:
            fprintf(fp, "%s ", "=");
            break;
        case BINOP_NE:
            fprintf(fp, "%s ", "!=");
            break;
        case BINOP_LT:
            fprintf(fp, "%s ", "<");
            break;
        case BINOP_LE:
            fprintf(fp, "%s ", "<=");
            break;
        case BINOP_GT:
            fprintf(fp, "%s ", ">");
            break;
        case BINOP_GE:
            fprintf(fp, "%s ", ">=");
            break;
        case BINOP_OR:
            fprintf(fp, "%s ", "or");
            break;
        case BINOP_AND:
            fprintf(fp, "%s ", "and");
            break;
        default:
            fprintf(fp, "%s ", "UNKNOWN");
            break;
    }
}
