#include <stdio.h>
#include "ast.h"

/* #extern void report_error_and_exit(const char *msg); */

/*
static const int INDENT = 4;
static const int INDENT_START = 0;
const char *binopname[] = {BINOP_NAMES};
const char *unopname[] = {UNOP_NAMES};
*/
void *checked_malloc(int num_bytes);

void gen_procs(FILE *fp, int indent, Procs procs);
void gen_header(FILE *fp, int *curr_reg, Header header);
void gen_params(FILE *fp, Params params);
void gen_decls(FILE *fp, int *curr_reg, Decls decls);
void gen_type(FILE *fp, VType type);
void gen_varnames(FILE *fp, int *curr_reg, VType type, VarNames varnames);
void gen_statements(FILE *fp, int *curr_reg, Stmts stmts);
void gen_statement(FILE *fp, int *curr_reg, Stmt stmt);
void gen_expressions(FILE *fp, int *curr_reg, Exprs exprs);
void gen_expression(FILE *fp, int *curr_reg, Expr expr);
void print_instruction(FILE *fp, Instr instr);
char *gen_nextreg(int *curr_reg); 

void
gen_prog(FILE *fp, Program prog) {

    /* report_error_and_exit("Unable to pretty-print"); */

    int indent = 0;
    gen_procs(fp, indent, prog->procs);
}

void
gen_procs(FILE *fp, int indent, Procs procs) {

    int     curr_reg = 0;

    /* fprintf(fp, "%s", "proc_");  */
    gen_header(fp, &curr_reg, procs->p_first->p_header);

    if(procs->p_first->p_decls != NULL)
        gen_decls(fp, &curr_reg, procs->p_first->p_decls);
    
    gen_statements(fp, &curr_reg, procs->p_first->p_body);
    
    fprintf(fp, "    pop_stack_frame X:\n" );
    fprintf(fp, "    return\n" );
    if(procs->p_rest != NULL)
        gen_procs(fp, indent, procs->p_rest);

}

void 
gen_header(FILE *fp, int *curr_reg, Header header) {

    fprintf(fp, "proc_%s:\n", header->h_id);
    fprintf(fp, "    push_stack_frame X:\n" );
   
    if (header->h_params != NULL) 
        gen_params(fp, header->h_params);

}

void 
gen_params(FILE *fp, Params params) {
/*
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

    gen_type(fp, params->p_first->d_type);
*/
    fprintf(fp, "# argument %s is in stack slot X\n", 
            params->p_first->d_id);

    if (params->p_rest != NULL) {
        /* fprintf(fp, ", "); */
        gen_params(fp, params->p_rest);
    }
}

void
gen_decls(FILE *fp, int *curr_reg, Decls decls) {

    /* gen_type(fp, decls->d_first->d_type);*/
    gen_varnames(fp, curr_reg, decls->d_first->d_type, decls->d_first->d_varnames);

    if (decls->d_rest != NULL) 
        gen_decls(fp, curr_reg, decls->d_rest); 
}

void
gen_varnames(FILE *fp, int *curr_reg, VType type, VarNames varnames) {


    fprintf(fp, "# variable %s is in stack slot Y\n", 
            varnames->v_first->v_id);
        
    /* gen_type(fp, type); */

    varnames->v_first->v_code = checked_malloc(sizeof(struct s_instr));
    varnames->v_first->v_code->op = STORE;
    varnames->v_first->v_code->arg1 = "Z";
    varnames->v_first->v_code->arg2 = gen_nextreg(curr_reg);

    print_instruction(fp, varnames->v_first->v_code);

    if (varnames->v_rest != NULL) {
        gen_varnames(fp, curr_reg, type, varnames->v_rest);
    }
}

void
gen_type(FILE *fp, VType type) {
    switch (type) {
        case INT:
            /* fprintf(fp, "%s ", "int"); */
            fprintf(fp, "%s ", "int"); 
            break;
        case FLOAT:
            /* fprintf(fp, "%s ", "float"); */
            break;
    }
}

void
gen_statements(FILE *fp, int *curr_reg, Stmts stmts) {

    /* fprintf(fp, "curr_reg address: %p\ncurr_reg value: %d\n", curr_reg, 
     * *curr_reg); */

    if (stmts->s_first != NULL) {
        gen_statement(fp, curr_reg, stmts->s_first); 
    }

    if (stmts->s_rest != NULL) {
        gen_statements(fp, curr_reg, stmts->s_rest); 
    }
}

void
print_instruction(FILE *fp, Instr instr) {

    const char *instr_opnames[] = {INSTR_OPNAMES};
    
    fprintf(fp, "    %s ", instr_opnames[instr->op]);
    if (instr->arg1 != NULL) fprintf(fp, "%s ", instr->arg1);
    if (instr->arg2 != NULL) fprintf(fp, "%s ", instr->arg2);
    if (instr->arg3 != NULL) fprintf(fp, "%s ", instr->arg3);
    fprintf(fp, "\n");
}

char
*gen_nextreg(int *curr_reg) {
    char    *reg = checked_malloc(sizeof(char[5]));

    sprintf(reg, "r%d", *curr_reg);
    *curr_reg = *curr_reg + 1;

    return reg;
}

void
gen_statement(FILE *fp, int *curr_reg, Stmt stmt) {

    /* fprintf(fp, "curr_reg address: %p\ncurr_reg value: %d\n", curr_reg, 
    * *curr_reg); */

    SKind s_kind = stmt->s_kind;

    switch (s_kind) {
        case STMT_ASSIGN:
            /* fprintf(fp, "%s := ", stmt->s_info.s_assign.asg_id); */
            fprintf(fp, "# assignment\n"); 
            gen_expression(fp, curr_reg, stmt->s_info.s_assign.asg_expr); 
            stmt->s_code = checked_malloc(sizeof(struct s_instr));
            stmt->s_code->op = STORE;
            stmt->s_code->arg1 = "1";
            stmt->s_code->arg2 = gen_nextreg(curr_reg);
            print_instruction(fp, stmt->s_code);
            /* fprintf(fp, "next reg: r%d\n", *curr_reg); */
            break;
        case STMT_BLOCK:
            /* fprintf(fp, "{\n"); */
            gen_statements(fp, curr_reg, stmt->s_info.s_block);
            /* fprintf(fp, "\n}"); */
            break;
        case STMT_COND:
            fprintf(fp, "%s ", "if");
            gen_expression(fp, curr_reg, stmt->s_info.s_cond.if_cond);
            fprintf(fp, " %s ", "then");
            gen_statement(fp, curr_reg, stmt->s_info.s_cond.if_then);
            fprintf(fp, "%s ", "\nelse");
            gen_statement(fp, curr_reg, stmt->s_info.s_cond.if_else);
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
            gen_expression(fp, curr_reg, stmt->s_info.s_while.while_cond);
            fprintf(fp, " %s ", "do");
            gen_statement(fp, curr_reg, stmt->s_info.s_while.while_body);
            break;
        case STMT_WRITE:
            fprintf(fp, "%s ", "write");
            gen_expression(fp, curr_reg, stmt->s_info.s_write);
            break;
        case STMT_FOR:
            fprintf(fp, "%s ", "for");
            fprintf(fp, "%s ", stmt->s_info.s_for.for_id);
            gen_expression(fp, curr_reg, stmt->s_info.s_for.for_from_expr);
            fprintf(fp, " %s ", "do");
            gen_expression(fp, curr_reg, stmt->s_info.s_for.for_to_expr);
            gen_statement(fp, curr_reg, stmt->s_info.s_for.for_body);
            break;
        case STMT_CALL:
            fprintf(fp, "%s(", stmt->s_info.s_call.call_id);
            gen_expressions(fp, curr_reg, stmt->s_info.s_call.s_exprs);
            fprintf(fp, ")");
            break;
    }
}

void
gen_expressions(FILE *fp, int *curr_reg, Exprs exprs) {
    /* only used in Calls ? */
    gen_expression(fp, curr_reg, exprs->e_first);
        
    if (exprs->e_rest != NULL) {
        fprintf(fp, ", ");
        gen_expressions(fp, curr_reg, exprs->e_rest);
    }
}

void
gen_expression(FILE *fp, int *curr_reg, Expr expr) {

    EKind e_kind = expr->e_kind;

    switch (e_kind) {
        case EXPR_ID:
            /* fprintf(fp, "%s", expr->e_id); */
            expr->e_code = checked_malloc(sizeof(struct s_instr));
            expr->e_code->op = LOAD;
            expr->e_code->arg1 = gen_nextreg(curr_reg);
            expr->e_code->arg2 = "1";
            print_instruction(fp, expr->e_code);
            break;
        case EXPR_INTCONST:
            /* fprintf(fp, "%d", expr->e_val); */
            expr->e_code = checked_malloc(sizeof(struct s_instr));
            expr->e_code->op = INT_CONST;
            expr->e_code->arg1 = gen_nextreg(curr_reg);
            expr->e_code->arg2 = "31";
            print_instruction(fp, expr->e_code);
            break;
        case EXPR_FLTCONST:
            fprintf(fp, "%.2f", expr->e_float);
            expr->e_code = checked_malloc(sizeof(struct s_instr));
            expr->e_code->op = REAL_CONST;
            expr->e_code->arg1 = gen_nextreg(curr_reg);
            expr->e_code->arg2 = "41";
            print_instruction(fp, expr->e_code);
            break;
        case EXPR_BINOP:
            gen_expression(fp, curr_reg, expr->e1);
            /* fprintf(fp, " %s ", binopname[expr->e_binop]);*/
            gen_expression(fp, curr_reg, expr->e2);
            break;
        case EXPR_UNOP:
            fprintf(fp, " %s", unopname[expr->e_unop]);
            gen_expression(fp, curr_reg, expr->e1);
            break;
    }
}

