/* parser.y */

/*---------------------------------------------------------------------------
    A bison spec for a small language, n7, for use in 433-361 asg 1, 2007.
    Harald Sondergaard, August 2007.
---------------------------------------------------------------------------*/

%{

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "std.h"
#include "missing.h"

extern Program parsed_program;
extern void    report_error_and_exit(const char *msg);
extern char    *yytext;

int ln = 1;
void yyerror(const char *msg);
void *allocate(int size);

%}

%union {
    int         int_val;
    char        *str_val;
    Decl        decl_val;
    Decls       decls_val;
    VarName     varname_val;
    VarNames    varnames_val;
    Param       param_val;
    Params      params_val;
    Expr        expr_val;
    Stmts       stmts_val;
    Stmt        stmt_val;
    VType       type_val;
    PKind       pkind_val;
    Header      hdr_val;
    Proc        proc_val;
    Procs       procs_val;
    Program     prog_val;
}

%token '(' ')' ',' ';' '{' '}'
%token ASSIGN_TOKEN DO_TOKEN   ELSE_TOKEN  IF_TOKEN    INT_TOKEN    FLOAT_TOKEN
%token READ_TOKEN   SKIP_TOKEN THEN_TOKEN  WHILE_TOKEN WRITE_TOKEN
%token PROC_TOKEN   END_TOKEN
%token VAL_TOKEN    REF_TOKEN  VALRES_TOKEN
%token INVALID_TOKEN
%token <int_val> NUMBER_TOKEN
%token <str_val> IDENT_TOKEN

/* Standard operator precedence */

%left '+' '-' 
%left '*'
%left UNARY_MINUS

%type <prog_val>    program
%type <procs_val>   procedure_list
%type <proc_val>    procedure
%type <hdr_val>     header 
%type <pkind_val>   passing_indicator
%type <type_val>    type
%type <param_val>   param_decl
%type <params_val>  param_decl_list

%type <decl_val>    decl
%type <decls_val>   decl_list
%type <varname_val>  var_name
%type <varnames_val> var_name_list

%type <stmts_val>   statement_list 
%type <stmt_val>    statement 
%type <expr_val>    expression 

%type <int_val>   assign
%type <int_val>   start_block
%type <int_val>   start_cond
%type <int_val>   start_read
%type <int_val>   start_while
%type <int_val>   start_write
%type <int_val>   get_lineno

%start program

%%
/*---------------------------------------------------------------------------*/

program 
    : procedure_list
        { 
          parsed_program = allocate(sizeof(struct s_prog));
          parsed_program->procs = $1;
        }
    ;

procedure_list
    : procedure procedure_list
	    {
          $$ = allocate(sizeof(struct s_proc));
          $$->p_first = $1;
          $$->p_rest = $2;
	    }
    | procedure
	    {
          $$ = allocate(sizeof(struct s_proc));
          $$->p_first = $1;
          $$->p_rest = NULL;
	    }
    ;

procedure
    : PROC_TOKEN header decl_list statement_list END_TOKEN
	    {
          $$ = allocate(sizeof(struct s_proc));
          $$->p_header = $2;
          $$->p_decls = $3;
          $$->p_body = $4;
	    }

header
    : IDENT_TOKEN '(' param_decl_list ')'
	    {
          $$ = allocate(sizeof(struct s_header));
          $$->h_id = $1;
          $$->h_params = $3;
	    }
    ;

param_decl_list 
    : param_decl
        {
          $$ = allocate(sizeof(struct s_params));
          $$->p_first = $1;
          $$->p_rest = NULL;
        }

    | param_decl ',' param_decl_list
        {
          $$ = allocate(sizeof(struct s_params));
          $$->p_first = $1;
          $$->p_rest = $3;
        }

    | /* empty */
        { $$ = NULL; }

    | error ',' param_decl_list
        { $$ = NULL; }
    ;

param_decl
    : passing_indicator type IDENT_TOKEN 
        { 
          $$ = allocate(sizeof(struct s_param));
          $$->d_kind = $1;
          $$->d_type = $2;
          $$->d_id = $3;
        }

    | /* empty */
        { $$ = NULL; }
    ;

passing_indicator
    : VALRES_TOKEN
        { $$ = VALRES; }
        
    | REF_TOKEN
        { $$ = REF; }

    | VAL_TOKEN
        { $$ = VAL; }
    ;

type
    : INT_TOKEN
        { $$ = INT; }
        
    | FLOAT_TOKEN
        { $$ = FLOAT; }
    ;

decl_list
    : decl ';' 
        { 
          $$ = allocate(sizeof(struct s_decls));
          $$->d_first = $1; 
          $$->d_rest = NULL; 
        }

    | decl ';' decl_list
        { 
          $$ = allocate(sizeof(struct s_decls));
          $$->d_first = $1; 
          $$->d_rest = $3; 
        }

    | /* empty */
        { $$ = NULL; }

    | error ';'
        { $$ = NULL; }
    ;


decl
    : type var_name_list
        { 
          $$ = allocate(sizeof(struct s_decl));
          $$->d_type = $1; 
          $$->d_varnames = $2; 
        }



var_name_list 
    : var_name
        {
          $$ = allocate(sizeof(struct s_varnames));
          $$->v_first = $1;
          $$->v_rest = NULL;
        }

    | var_name ',' var_name_list
        {
          $$ = allocate(sizeof(struct s_varnames));
          $$->v_first = $1;
          $$->v_rest = $3;
        }

    | error ',' var_name_list
        { $$ = NULL; }
    ;

var_name
    : IDENT_TOKEN
        { 
          $$ = allocate(sizeof(struct s_varname));
          /* $$->d_lineno = ln; */
          $$->v_id = $1;
        }
    ;

get_lineno
    : /* empty */
        { $$ = ln; }

assign
    : ASSIGN_TOKEN
        { $$ = ln; }

start_block
    : '{'
        { $$ = ln; }

start_cond
    : IF_TOKEN
        { $$ = ln; }

start_read
    : READ_TOKEN
        { $$ = ln; }

start_while
    : WHILE_TOKEN
        { $$ = ln; }

start_write
    : WRITE_TOKEN
        { $$ = ln; }

statement_list                           /* non-empty list of statements */
    : statement ';' statement_list
        {
          $$ = allocate(sizeof(struct s_stmts));
          $$->s_first = $1;
          $$->s_rest = $3;
        }

    | statement ';' statement
        {
          $$ = allocate(sizeof(struct s_stmts));
          $$->s_first = $1;
          $$->s_rest = allocate(sizeof(struct s_stmts));
          $$->s_rest->s_first = $3;
          $$->s_rest->s_rest= NULL;
        }

    | statement 
        {
          $$ = allocate(sizeof(struct s_stmts));
          $$->s_first = $1;
          $$->s_rest = NULL;
        }

    | error ';' { yyerrok; } statement_list
        { $$ = $4; }
    ;

statement 
    : start_block statement_list '}'
        {
          $$ = allocate(sizeof(struct s_stmt));
          $$->s_lineno = $1;
          $$->s_kind = STMT_BLOCK;
          $$->s_info.s_block = $2;
        }

    | IDENT_TOKEN assign expression
        {
          $$ = allocate(sizeof(struct s_stmt));
          $$->s_lineno = $2;
          $$->s_kind = STMT_ASSIGN;
          $$->s_info.s_assign.asg_id = $1;
          $$->s_info.s_assign.asg_expr = $3;
        }

    | SKIP_TOKEN
        { $$ = allocate(sizeof(struct s_stmt));
          $$->s_lineno = ln;
          $$->s_kind = STMT_SKIP;
        }

    | start_read IDENT_TOKEN
        {
          $$ = allocate(sizeof(struct s_stmt));
          $$->s_lineno = $1;
          $$->s_kind = STMT_READ;
          $$->s_info.s_read = $2;
        }

    | start_write expression 
        {
          $$ = allocate(sizeof(struct s_stmt));
          $$->s_lineno = $1;
          $$->s_kind = STMT_WRITE;
          $$->s_info.s_write = $2;
        }

    | start_cond expression THEN_TOKEN statement ELSE_TOKEN statement
        {
          $$ = allocate(sizeof(struct s_stmt));
          $$->s_lineno = $1;
          $$->s_kind = STMT_COND;
          $$->s_info.s_cond.if_cond = $2;
          $$->s_info.s_cond.if_then = $4;
          $$->s_info.s_cond.if_else = $6;
        }

    | start_while expression DO_TOKEN statement
        {
          $$ = allocate(sizeof(struct s_stmt));
          $$->s_lineno = $1;
          $$->s_kind = STMT_WHILE;
          $$->s_info.s_while.while_cond = $2;
          $$->s_info.s_while.while_body = $4;
        }
    ;

expression 
    : '-' get_lineno expression                %prec UNARY_MINUS
        {
          $$ = allocate(sizeof(struct s_expr));
          $$->e_kind = EXPR_UNOP;
          $$->e_unop = UNOP_MINUS;
          $$->e1 = $3;
          $$->e2 = NULL;
          $$->e_lineno = $2;
        }

    | expression '+' get_lineno expression
        {
          $$ = allocate(sizeof(struct s_expr));
          $$->e_kind = EXPR_BINOP;
          $$->e_binop = BINOP_ADD;
          $$->e1 = $1;
          $$->e2 = $4;
          $$->e_lineno = $1->e_lineno == $4->e_lineno ? $1->e_lineno : $3;
        }

    | expression '-' get_lineno expression
        {
          $$ = allocate(sizeof(struct s_expr));
          $$->e_kind = EXPR_BINOP;
          $$->e_binop = BINOP_SUB;
          $$->e1 = $1;
          $$->e2 = $4;
          $$->e_lineno = $1->e_lineno == $4->e_lineno ? $1->e_lineno : $3;
        }

    | expression '*' get_lineno expression
        {
          $$ = allocate(sizeof(struct s_expr));
          $$->e_kind = EXPR_BINOP;
          $$->e_binop = BINOP_MUL;
          $$->e1 = $1;
          $$->e2 = $4;
          $$->e_lineno = $1->e_lineno == $4->e_lineno ? $1->e_lineno : $3;
        }

    | '(' expression ')'
        {
          $$ = $2;
        }
/*
    | '(' expression ')'
        {
          $$ = allocate(sizeof(struct s_expr));
          $$->e_lineno = ln;
          $$->e_kind = EXPR_PAREN;
          $$->e1 = $2;
          $$->e2 = NULL;
        }
*/

    | '(' error ')'
        { $$ = NULL; }

    | IDENT_TOKEN
        { 
          $$ = allocate(sizeof(struct s_expr));
          $$->e_lineno = ln;
          $$->e_kind = EXPR_ID;
          $$->e_id = $1;
          $$->e2 = NULL;
          $$->e2 = NULL;
        }

    | NUMBER_TOKEN
        {
          $$ = allocate(sizeof(struct s_expr));
          $$->e_lineno = ln;
          $$->e_kind = EXPR_CONST;
          $$->e_val = $1;
          $$->e2 = NULL;
          $$->e2 = NULL;
        }
    ;

%%

/*---------------------------------------------------------------------------*/

void yyerror(const char *msg) {

    fprintf(stderr, "**** Input line %d, near `%s': %s\n", ln, yytext, msg);
    return;
}

void *
allocate(int size) {

    void    *addr;

    addr = malloc((size_t) size);
    if (addr == NULL) 
        report_error_and_exit("Out of memory");
    return addr;
}

/*---------------------------------------------------------------------------*/

