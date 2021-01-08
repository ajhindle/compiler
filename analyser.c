/*
 * Author   : ajhindle
 * Date     : 25-11-2020
 *
 * File     : analyser.c
 *
 * This module runs semantic analysis over the AST.
*/

#include <stdio.h>
#include "ast.h"
#include "traverse.h"
#include "symbol.h"
#include "stack.h"

extern void report_error_and_exit(const char *msg);


void analyse_proc(FILE *fp, Proc proc);
void analyse_header(FILE *fp, Header header);
void analyse_params(FILE *fp, Params params);
void analyse_decl(FILE *fp, Decl decl);
void analyse_type(FILE *fp, VType type);
void analyse_varnames(FILE *fp, VType type, VarNames varnames);
void analyse_statements(FILE *fp, Stmts stmts);
void analyse_statement(FILE *fp, Stmt stmt);
void analyse_expressions(FILE *fp, Exprs exprs);
void analyse_expression(FILE *fp, Expr expr);
void analyse_binop(FILE *fp, Expr expr);
void set_op(FILE *fp, Expr expr); 

static int         param_ct;
static int         var_ct;
static int         slot_ct;
static SymbolTbl   *prog_st;
static Stack       stack; 

static const char  *vtypes[] = {VTYPE};
static const char  *btypes[] = {BINOP_NAMES};

void
analyse_prog(FILE *fp, Program prog) {

    prog_st = st_init(31);
    prog->prog_st = prog_st;

    stack = stack_init();

    proc_procs(fp, analyse_proc, prog->procs);

    st_dump(prog->prog_st);
}

void
analyse_proc(FILE *fp, Proc proc) {

    slot_ct = 0;
    param_ct = 0;
    var_ct = 0;

    Frame frame = push(stack);
    frame->st = st_init(31);
    proc->p_st = frame->st;

    proc_header(fp, analyse_header, proc->p_header);
    
    proc->p_param_ct = param_ct;
    proc->p_var_ct = var_ct;

    if(proc->p_decls != NULL) {
        proc_decls(fp, analyse_decl, proc->p_decls);
        proc->p_var_ct = var_ct;
    }
    
    proc_statements(fp, analyse_statements, proc->p_body);

    proc->p_slot_ct = slot_ct;

    frame = pop(stack);
    st_dump(frame->st);
}

void 
analyse_header(FILE *fp, Header header) {

    st_insert(prog_st, header->h_id);
   
    if (header->h_params != NULL) 
        proc_params(fp, analyse_params, header->h_params);
}

void 
analyse_params(FILE *fp, Params params) {

    int     pos;
    Param   param = params->p_first;

    param_ct = param_ct + 1;

    switch (param->d_kind) {
        case VAL:
            break;
        case VALRES:
            break;
        case REF:
            break;
    }

    pos = st_insert(stack->top->st, param->d_id);
    //add the type to symbol table too
    stack->top->st->s_items[pos].type = param->d_type;
    stack->top->st->s_items[pos].stack_slot = slot_ct;
    //decl: d_type
    //proc: no type
    slot_ct = slot_ct + 1;

    analyse_type(fp, param->d_type);

    if (params->p_rest != NULL) {
        proc_params(fp, analyse_params, params->p_rest);
    }
}

void
analyse_decl(FILE *fp, Decl decl) {

    var_ct = var_ct + 1;
    //analyse_type(fp, decl->d_type);
    analyse_varnames(fp, decl->d_type, decl->d_varnames);
}

void
analyse_varnames(FILE *fp, VType type, VarNames varnames) {

    int     pos;
    VarName varname = varnames->v_first;

    fprintf(fp, "Varname is %s\n", varname->v_id);
    
    pos = st_insert(stack->top->st, varname->v_id);
    //add the type to symbol table too
    stack->top->st->s_items[pos].type = type;
    stack->top->st->s_items[pos].stack_slot = slot_ct;
    slot_ct = slot_ct + 1;

    if (varnames->v_rest != NULL) 
        analyse_varnames(fp, type, varnames->v_rest);

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
        proc_statement(fp, analyse_statement, stmts->s_first); 
    }

    if (stmts->s_rest != NULL) {
        proc_statements(fp, analyse_statements, stmts->s_rest); 
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

    int     pos;
    EKind   e_kind = expr->e_kind;


    switch (e_kind) {
        case EXPR_ID:
            //lookup the symbol table to find the expr_id type
            //TODO handler for undeclared var
            pos = st_lookup(stack->top->st, expr->e_id);
            expr->e_type = stack->top->st->s_items[pos].type;
            fprintf(fp, "Line %d: ID %s is type %s\n", 
                    expr->e_lineno,
                    expr->e_id, 
                    vtypes[stack->top->st->s_items[pos].type]);
            break;
        case EXPR_INTCONST:
            expr->e_type = INT;
            //fprintf(fp, "INTCONST value is %d\n", expr->e_intval);
            fprintf(fp, "Line %d: '%d' is an INTCONST\n", 
                    expr->e_lineno,
                    expr->e_intval); 
            break;
        case EXPR_FLTCONST:
            expr->e_type = FLOAT;
            //fprintf(fp, "FLOATCONST value is %.3f\n", expr->e_fltval);
            fprintf(fp, "Line %d: '%.3f' is a FLOATCONST\n", 
                    expr->e_lineno,
                    expr->e_fltval); 
            break;
        case EXPR_BINOP:
            analyse_binop(fp, expr); 
            break;
        case EXPR_UNOP:
            analyse_expression(fp, expr->e1);
            break;
        default: 
            break;
    }
}


/*
 * Determines the expression BINOP type based on what the two sub-expressions 
 * are.
 */
void
analyse_binop(FILE *fp, Expr expr) {

    analyse_expression(fp, expr->e1);
    analyse_expression(fp, expr->e2);
    if (expr->e1->e_type == INT && expr->e2->e_type == INT) {
        expr->e_type = INT;
        fprintf(fp, "Line %d: '%s' is type %s\n", 
                expr->e_lineno,
                btypes[expr->e_binop],
                vtypes[expr->e_type]); 
    } 
    else {
        expr->e_type = FLOAT;
        fprintf(fp, "Line %d: '%s' is type %s\n", 
                expr->e_lineno,
                btypes[expr->e_binop],
                vtypes[expr->e_type]); 
    }
    set_op(fp, expr);

}


/*
 * Determines the BINOP 'argument' to be used later for three-address-code.
 * This procedure somewhat does part of the code generator's job. 
 */
void
set_op(FILE *fp, Expr expr) {

    if (expr->e_type == INT) {
        switch (expr->e_binop) {
            case BINOP_ADD:
                expr->e_code->op = ADD_INT;
                break;
            case BINOP_SUB:
                expr->e_code->op = SUB_INT;
                break;
            case BINOP_MUL:
                expr->e_code->op = MUL_INT;
                break;
            case BINOP_DIV:
                expr->e_code->op = DIV_INT;
                break;
            case BINOP_EQ:
                expr->e_code->op = CMP_EQ_INT;
                break;
            case BINOP_NE:
                expr->e_code->op = CMP_NE_INT;
                break;
            case BINOP_GT:
                expr->e_code->op = CMP_GT_INT;
                break;
            case BINOP_GE:
                expr->e_code->op = CMP_GE_INT;
                break;
            case BINOP_LT:
                expr->e_code->op = CMP_LT_INT;
                break;
            case BINOP_LE:
                expr->e_code->op = CMP_LE_INT;
                break;
            case BINOP_OR:
                expr->e_code->op = OR;
                break;
            case BINOP_AND:
                expr->e_code->op = AND;
                break;
        }
    }
    else {
        switch (expr->e_binop) {
            case BINOP_ADD:
                expr->e_code->op = ADD_REAL;
                break;
            case BINOP_SUB:
                expr->e_code->op = SUB_REAL;
                break;
            case BINOP_MUL:
                expr->e_code->op = MUL_REAL;
                break;
            case BINOP_DIV:
                expr->e_code->op = DIV_REAL;
                break;
            case BINOP_EQ:
                expr->e_code->op = CMP_EQ_REAL;
                break;
            case BINOP_NE:
                expr->e_code->op = CMP_NE_REAL;
                break;
            case BINOP_GT:
                expr->e_code->op = CMP_GT_REAL;
                break;
            case BINOP_GE:
                expr->e_code->op = CMP_GE_REAL;
                break;
            case BINOP_LT:
                expr->e_code->op = CMP_LT_REAL;
                break;
            case BINOP_LE:
                expr->e_code->op = CMP_LE_REAL;
                break;
            case BINOP_OR:
                expr->e_code->op = OR;
                break;
            case BINOP_AND:
                expr->e_code->op = AND;
        }
    }
}
