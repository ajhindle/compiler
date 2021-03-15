/*
 * Author   : ajhindle
 * Date     : 25-11-2020
 *
 * File     : codegen.c
 *
 * This module provides procedures that translate the input program into 
 * intermediate instructions (IR).  
*/


#include <stdio.h>
#include "ast.h"
#include "util.h"
#include "traverse.h"


void gen_proc(FILE *fp, Proc proc);
void gen_header(FILE *fp, Header header);
void gen_params(FILE *fp, Params params);
void gen_decl(FILE *fp, Decl decl);
void gen_varname(FILE *fp, VarName varname);
void gen_statements(FILE *fp, Stmts stmts);
void gen_statement(FILE *fp, Stmt stmt);
void gen_expressions(FILE *fp, Exprs exprs);
void gen_expression(FILE *fp, Expr expr);
void print_instruction(FILE *fp, Instr instr);
void print_instr_arg(FILE *fp, Arg arg); 
void get_nextplace(Arg arg, AType a_type);
void check_type_mismatch(FILE *fp, Expr expr); 
void gen_convert_to_real(FILE *fp, Expr expr);

static int      curr_reg;
static int      curr_slot;
static int      curr_label;
static Proc     curr_proc;
static VType    var_type;
const char      *builtin_names[] = {BUILTIN_NAMES};

void
gen_prog(FILE *fp, Program prog) {

    curr_label = 0;
    fprintf(fp, "    call proc_main\n");
    fprintf(fp, "    halt\n");
    proc_procs(fp, gen_proc, prog->procs);
}

void
gen_proc(FILE *fp, Proc proc) {

    curr_reg = 0;
    curr_slot = 0;

    curr_proc = proc;

    if (proc->p_main == 1)
        fprintf(fp, "# p_main = 1\n");
    else
        fprintf(fp, "# p_main = 0\n");

    fprintf(fp, "proc_%s:\n", proc->p_header->h_id);
    fprintf(fp, "    push_stack_frame %d\n", proc->p_slot_ct);
    proc_header(fp, gen_header, proc->p_header);

    if(proc->p_decls != NULL)
        proc_decls(fp, gen_decl, proc->p_decls);
    
    gen_statements(fp, proc->p_body);
    
    fprintf(fp, "    pop_stack_frame %d\n", proc->p_slot_ct);
    fprintf(fp, "    return\n" );
}

void 
gen_header(FILE *fp, Header header) {

    if (header->h_params != NULL) 
        proc_params(fp, gen_params, header->h_params);
}

void 
gen_params(FILE *fp, Params params) {

    Instr   p_code = params->p_first->p_code;
    int     pos;

    p_code->op = STORE;
    pos = st_lookup(curr_proc->p_st, params->p_first->d_id);
    p_code->arg1->a_val = curr_proc->p_st->s_items[pos].stack_slot;
    p_code->arg1->a_type = SLOT;

    get_nextplace(p_code->arg2, REG);
    fprintf(fp, "# argument %s is in stack slot %d\n", 
            params->p_first->d_id, p_code->arg1->a_val);

    print_instruction(fp, p_code);

    if (params->p_rest != NULL) 
        proc_params(fp, gen_params, params->p_rest);
    
}

void
gen_decl(FILE *fp, Decl decl) {

    var_type = decl->d_type;

    /* gen_type(fp, decls->d_first->d_type);*/
    proc_varnames(fp, gen_varname, decl->d_varnames);

}

void
gen_varname(FILE *fp, VarName varname) {

    Instr   v_code = varname->v_code;
    int     pos;

    pos = st_lookup(curr_proc->p_st, varname->v_id);
    varname->v_code->arg1->a_val = curr_proc->p_st->s_items[pos].stack_slot;
    varname->v_code->arg1->a_type = SLOT;


    fprintf(fp, "# variable %s is in stack slot %d\n", 
            varname->v_id, v_code->arg1->a_val);

    // initialise zero value in register
    switch (var_type) {
        case INT:
            v_code->op = INT_CONST;
            v_code->arg2->a_type = INTCONST;
            break;
        case FLOAT:
            v_code->op = REAL_CONST;
            v_code->arg2->a_type = REALCONST;
            break;
    }
    get_nextplace(v_code->arg1, REG);
    v_code->arg2->a_val = 0;
    print_instruction(fp, varname->v_code);
   
    // store register value in slot
    v_code->op = STORE;
    v_code->arg2->a_val = v_code->arg1->a_val;
    v_code->arg2->a_type = REG;
    v_code->arg1->a_type = SLOT;
    v_code->arg1->a_val = curr_proc->p_st->s_items[pos].stack_slot;

    print_instruction(fp, varname->v_code);
}

void
gen_statements(FILE *fp, Stmts stmts) {

    if (stmts->s_first != NULL) {
        proc_statement(fp, gen_statement, stmts->s_first); 
    }

    // TODO use Traverse for this instead
    if (stmts->s_rest != NULL) {
        gen_statements(fp, stmts->s_rest); 
    }
}

/*
 * Prints 'three address code' 
 */
void
print_instruction(FILE *fp, Instr instr) {

    const char *instr_opnames[] = {INSTR_OPNAMES};
    
    fprintf(fp, "    %s ", instr_opnames[instr->op]);
    if (instr->num_args >= 1) 
        print_instr_arg(fp, instr->arg1);
    if (instr->num_args >= 2) {
        fprintf(fp, ", ");
        print_instr_arg(fp, instr->arg2);
    }
    if (instr->num_args >= 3) {
        fprintf(fp, ", ");
        print_instr_arg(fp, instr->arg3);
    }
    fprintf(fp, "\n");
}

void
print_instr_arg(FILE *fp, Arg arg) {
    switch (arg->a_type) {
        case REG:
            fprintf(fp, "r%d", arg->a_val);
            break;
        case SLOT:
            fprintf(fp, "%d", arg->a_val);
            break;
        case INTCONST:
            fprintf(fp, "%d", arg->a_val);
            break;
        case REALCONST:
            fprintf(fp, "%.5f", arg->a_fltval);
            break;
        case STRCONST:
            fprintf(fp, "%s", arg->a_strval);
            break;
        case BUILTIN:
            fprintf(fp, "%s", builtin_names[arg->a_val]);
            break;
        case LABEL:
            fprintf(fp, "label%d", arg->a_val);
            break;
    }
}


void
get_nextplace(Arg arg, AType a_type) {

    switch (a_type) {
        case REG:
            arg->a_val = curr_reg;
            arg->a_type = REG;
            curr_reg = curr_reg + 1;
            break;
        case LABEL:
            sprintf(arg->a_strval, "label%d", curr_label);
            arg->a_type = LABEL;
            curr_label = curr_label + 1;
            break;
        default:
            report_error_and_exit("Invalid instruction argument type."); 
            break;
    }
}


/*
 * Generates target code for a statement. Places/registers are determined here 
 * and provided to (inherited by) expressions within the statement. After code 
 * generation of an expression, the statement uses value found in the place 
 * where the expression left a result. Statements also use slot values which 
 * are found in the symbol table.
 */

void
gen_statement(FILE *fp, Stmt stmt) {

    int     pos;
    SKind   s_kind = stmt->s_kind;
    int     uncond_label, false_label, begin_label;

    switch (s_kind) {
        case STMT_ASSIGN:
            fprintf(fp, "# assignment\n"); 
            curr_reg = 0;
            
            stmt->s_code->op = STORE;
            
            // get the correct "slot"
            pos = st_lookup(curr_proc->p_st, stmt->s_info.s_assign.asg_id);
            stmt->s_code->arg1->a_val = curr_proc->p_st->s_items[pos].stack_slot;
            stmt->s_code->arg1->a_type = SLOT;

            // get the "place" (reg) of the expression
            get_nextplace(stmt->s_info.s_assign.asg_expr->e_place, REG);
            gen_expression(fp, stmt->s_info.s_assign.asg_expr); 
            stmt->s_code->arg2 = stmt->s_info.s_assign.asg_expr->e_place;

            print_instruction(fp, stmt->s_code);
            break;
        case STMT_BLOCK:
            fprintf(fp, "# block\n"); 
            gen_statements(fp, stmt->s_info.s_block);
            break;
        case STMT_COND:
            fprintf(fp, "# if\n"); 
            get_nextplace(stmt->s_info.s_cond.if_cond->e_place, REG);
            gen_expression(fp, stmt->s_info.s_cond.if_cond);

            // set up branching instructions and "jump-to" labels

            stmt->s_code->op = BRANCH_ON_FALSE;
            stmt->s_code->arg1 = stmt->s_info.s_cond.if_cond->e_place;
            false_label = curr_label;
            curr_label++;
            uncond_label = curr_label;
            curr_label++;
            stmt->s_code->arg2->a_val = false_label;
            stmt->s_code->arg2->a_type = LABEL;
            print_instruction(fp, stmt->s_code);

            // TRUE 
            gen_statement(fp, stmt->s_info.s_cond.if_then);
            
            stmt->s_code->num_args = 1;
            stmt->s_code->op = BRANCH_UNCOND;
            stmt->s_code->arg1->a_type = LABEL;
            stmt->s_code->arg1->a_val = uncond_label;
            print_instruction(fp, stmt->s_code);

            // FALSE 
            fprintf(fp, "label%d:\n", false_label);
            gen_statement(fp, stmt->s_info.s_cond.if_else);

            fprintf(fp, "label%d:\n", uncond_label);

            break;
        case STMT_READ:
            fprintf(fp, "# read\n"); 
            pos = st_lookup(curr_proc->p_st, stmt->s_info.s_read);
            stmt->s_code->op = CALL_BUILTIN;
            stmt->s_code->arg1->a_type = BUILTIN;

            switch (curr_proc->p_st->s_items[pos].type) {
                case E_TYPE_INT:
                    stmt->s_code->arg1->a_val = READ_INT;
                    break;
                case E_TYPE_FLOAT:
                    stmt->s_code->arg1->a_val = READ_REAL;
                    break;
                case E_TYPE_STRING:
                    stmt->s_code->arg1->a_val = READ_STRING;
            }

            print_instruction(fp, stmt->s_code);
            
            curr_reg = 0;
            stmt->s_code->num_args = 2;
            stmt->s_code->op = STORE;

            stmt->s_code->arg1->a_val = curr_proc->p_st->s_items[pos].stack_slot;
            stmt->s_code->arg1->a_type = SLOT;
            stmt->s_code->arg2->a_val = curr_reg;
            stmt->s_code->arg2->a_type = REG;
            print_instruction(fp, stmt->s_code);

            break;
        case STMT_SKIP:
            break;
        case STMT_WHILE:
            fprintf(fp, "# while\n"); 
            curr_reg = 0;
            get_nextplace(stmt->s_info.s_while.while_cond->e_place, REG);
            
            uncond_label = curr_label;
            curr_label++;
            begin_label = curr_label;
            curr_label++;

            // set up branching instructions and "jump-to" labels
            fprintf(fp, "label%d:\n", begin_label);
            gen_expression(fp, stmt->s_info.s_while.while_cond);

            stmt->s_code->num_args = 2;
            stmt->s_code->op = BRANCH_ON_FALSE;
            stmt->s_code->arg1 = stmt->s_info.s_while.while_cond->e_place;
            stmt->s_code->arg2->a_val = uncond_label;
            stmt->s_code->arg2->a_type = LABEL;
            print_instruction(fp, stmt->s_code);

            gen_statement(fp, stmt->s_info.s_while.while_body);

            stmt->s_code->num_args = 1;
            stmt->s_code->op = BRANCH_UNCOND;
            stmt->s_code->arg1->a_val = begin_label;
            stmt->s_code->arg1->a_type = LABEL;
            print_instruction(fp, stmt->s_code);

            fprintf(fp, "label%d:\n", uncond_label);

            break;
        case STMT_WRITE:
            fprintf(fp, "# write\n"); 
            curr_reg = 0;
            // set the place of the inner expression
            get_nextplace(stmt->s_info.s_write->e_place, REG);
            // set the slot of the inner expression
            gen_expression(fp, stmt->s_info.s_write);
            
            curr_reg = 0;
            
            stmt->s_code->op = CALL_BUILTIN;
            stmt->s_code->arg1->a_type = BUILTIN;

            switch (stmt->s_info.s_write->e_type) {
                case E_TYPE_INT:
                    stmt->s_code->arg1->a_val = PRINT_INT;
                    break;
                case E_TYPE_FLOAT:
                    stmt->s_code->arg1->a_val = PRINT_REAL;
                    break;
                case E_TYPE_STRING:
                    stmt->s_code->arg1->a_val = PRINT_STRING;
                    break;
            }

            print_instruction(fp, stmt->s_code);

            break;
        case STMT_FOR:
            //TODO
            gen_expression(fp, stmt->s_info.s_for.for_from_expr);
            gen_expression(fp, stmt->s_info.s_for.for_to_expr);
            gen_statement(fp, stmt->s_info.s_for.for_body);
            break;
        case STMT_CALL:
            // curr_reg = 0 ????;
            // get active registers and save them in stack slot for use with 
            // the called proc
            stmt->s_code->op = CALL;
            stmt->s_code->arg1->a_type = LABEL;
            stmt->s_code->arg1->a_strval = stmt->s_info.s_call.call_id;
            print_instruction(fp, stmt->s_code);
            gen_expressions(fp, stmt->s_info.s_call.s_exprs);
            // (required??) put stack slot values back in registers
            break;
    }
}

void
gen_expressions(FILE *fp, Exprs exprs) {
    /* handles a list of expressions only used in Calls */
    gen_expression(fp, exprs->e_first);
        
    if (exprs->e_rest != NULL) {
        fprintf(fp, ", ");
        gen_expressions(fp, exprs->e_rest);
    }
}

void
gen_expression(FILE *fp, Expr expr) {

    EKind   e_kind = expr->e_kind;
    int     pos;

    switch (e_kind) {
        case EXPR_ID:
            expr->e_code->op = LOAD;
            expr->e_code->arg1 = expr->e_place;
            pos = st_lookup(curr_proc->p_st, expr->e_id);
            expr->e_code->arg2->a_val = curr_proc->p_st->s_items[pos].stack_slot;
            expr->e_code->arg2->a_type = SLOT;
            print_instruction(fp, expr->e_code);
            break;
        case EXPR_INTCONST:
            expr->e_code->op = INT_CONST;
            expr->e_code->arg1 = expr->e_place;
            expr->e_code->arg2->a_type = INTCONST;
            expr->e_code->arg2->a_val = expr->e_intval; 
            print_instruction(fp, expr->e_code);
            break;
        case EXPR_FLTCONST:
            expr->e_code->op = REAL_CONST;
            expr->e_code->arg1 = expr->e_place;
            expr->e_code->arg2->a_type = REALCONST;
            expr->e_code->arg2->a_fltval = expr->e_fltval;
            print_instruction(fp, expr->e_code);
            break;
        case EXPR_STRCONST:
            expr->e_code->op = STRING_CONST;
            expr->e_code->arg1 = expr->e_place;
            expr->e_code->arg2->a_type = STRCONST;
            expr->e_code->arg2->a_strval = expr->e_id;
            print_instruction(fp, expr->e_code);
            break;
        case EXPR_BINOP:
            get_nextplace(expr->e1->e_place, REG);
            get_nextplace(expr->e2->e_place, REG);
            gen_expression(fp, expr->e1);
            gen_expression(fp, expr->e2);
            check_type_mismatch(fp, expr);
            expr->e_code->arg1 = expr->e_place;
            expr->e_code->arg2 = expr->e1->e_place;
            expr->e_code->arg3 = expr->e2->e_place;
            print_instruction(fp, expr->e_code);
            break;
        case EXPR_UNOP:
            get_nextplace(expr->e1->e_place, REG);
            gen_expression(fp, expr->e1);
            if (expr->e_unop == UNOP_NOT) {
                expr->e_code->op = NOT;
                expr->e_code->arg1->a_type = REG;
                expr->e_code->arg1->a_val = 0;
                expr->e_code->arg2 = expr->e1->e_place;
                print_instruction(fp, expr->e_code);
                break;
            }
            if (expr->e_unop == UNOP_MINUS) {
                report_error_and_exit("UNOP_MINUS processing. \
                        Not allowed in codegen."); 
                break;
            }

            break;
    }
}

/*
 * Checks if both sub expressions of a float binop expression are floats. If 
 * they are not then generate a conversion instruction. 
 * e.g. r1 has INT and r2 has REAL, therefore convert r1 to REAL 
 */
void
check_type_mismatch(FILE *fp, Expr expr) {

    if (expr->e_type == E_TYPE_FLOAT) {
        if (expr->e1->e_type != E_TYPE_FLOAT) 
            gen_convert_to_real(fp, expr->e1);
        
        if (expr->e2->e_type != E_TYPE_FLOAT) 
            gen_convert_to_real(fp, expr->e2);
    }
}

void
gen_convert_to_real(FILE *fp, Expr expr) {
    
    Instr   code = alloc_code(2);

    code->op = INT_TO_REAL;
    code->arg1 = expr->e_place;
    code->arg2 = expr->e_place;
    print_instruction(fp, code);
}
