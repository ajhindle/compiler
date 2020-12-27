/* ast.h */

/*---------------------------------------------------------------------------
    Definitions for the abstract syntax trees generated for n7 programs.
    For use in 433-361 asg 1, 2007.
    Harald Sondergaard, August 2007.
---------------------------------------------------------------------------*/



#ifndef AST_H
#define AST_H

#include "symbol.h"

typedef struct s_decl   	*Decl;
typedef struct s_decls 		*Decls;
typedef struct s_varname    *VarName;
typedef struct s_varnames 	*VarNames;
typedef struct s_param      *Param;
typedef struct s_params     *Params;
typedef struct s_expr   	*Expr;
typedef struct s_exprs		*Exprs;
typedef struct s_stmts  	*Stmts;
typedef struct s_stmt   	*Stmt;
typedef struct s_header		*Header;
typedef struct s_proc		*Proc;
typedef	struct s_procs		*Procs;
typedef struct s_prog   	*Program;

typedef	struct s_instr		*Instr;
typedef	struct s_arg		*Arg;


typedef enum {
    BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV, 
	BINOP_EQ, BINOP_NE, BINOP_LT, BINOP_LE, BINOP_GT, BINOP_GE,
	BINOP_OR, BINOP_AND
} BinOp;

/* typedef enum {
    ADD_INT, ADD_REAL, SUB_INT, SUB_REAL, MUL_INT, MUL_REAL,
    DIV_INT, DIV_REAL,
    AND, OR, 
    CMP_EQ_INT, CMP_NE_INT, CPM_GT_INT, CMP_GE_INT, 
    CMP_LT_INT, CMP_LE_INT, 
    CMP_EQ_REAL, CMP_NE_REAL, CPM_GT_REAL, CMP_GE_REAL 
} ;
*/


#define BINOP_NAMES "+", "-", "*", "/", "=", "!=", "<", "<=", \
    ">", ">=", "or", "and"

#define CG_BINOP_INT "add_int", "sub_int", "mul_int", "div_int", \
"cmp_eq_int", "cmp_ne_int", "cmp_lt_int", "cmp_le_int", \
"cmp_gt_int", "cmp_ge_int", "and", "or" 

#define CG_BINOP_REAL "add_real", "sub_real", "mul_real", "div_real", \
"cmp_eq_real", "cmp_ne_real", "cmp_lt_real", "cmp_le_real", \
"cmp_gt_real", "cmp_ge_real", "and", "or" 

#define VTYPE "INT", "FLOAT"

extern const char *binopname[];

typedef enum {
    UNOP_MINUS, UNOP_NOT
} UnOp;

#define UNOP_NAMES "-", "not"

extern const char *unopname[];

typedef enum {
    EXPR_ID, EXPR_INTCONST, EXPR_FLTCONST, EXPR_BINOP, EXPR_UNOP
} EKind;

typedef enum {
	VAL, REF, VALRES
} PKind;

typedef enum {
	INT, FLOAT
} VType;

typedef enum {
	REG, SLOT, INTCONST, REALCONST
} AType;

struct s_expr {
    int     e_lineno;
    EKind   e_kind;
    char    *e_id;      /* for ID */
    int     e_intval;   /* for integer values */
	float	e_fltval;	/* for float values */
    UnOp    e_unop;     /* for UNOP */
    BinOp   e_binop;    /* for BINOP */
    Expr    e1;         /* for UNOP and BINOP */
    Expr    e2;         /* for BINOP */
    Instr   e_code;     /* code generation instruction */
    Arg     e_place;    /* code generation place (register) */
    VType   e_type;     /* expression type */
};

typedef enum {
    ADD_INT, ADD_REAL, SUB_INT, SUB_REAL, 
    MUL_INT, MUL_REAL, DIV_INT, DIV_REAL,
    CMP_EQ_INT, CMP_NE_INT, CMP_GT_INT, CMP_GE_INT, 
    CMP_LT_INT, CMP_LE_INT, CMP_EQ_REAL, CMP_NE_REAL, 
    CMP_GT_REAL, CMP_GE_REAL, CMP_LT_REAL, CMP_LE_REAL, 
    AND, OR, NOT,
    INT_TO_REAL, MOVE,  
    PUSH_STACK_FRAME, POP_STACK_FRAME,
    LOAD, STORE, LOAD_ADDR, LOAD_IND, STORE_IND, 
    INT_CONST, REAL_CONST, STRING_CONST,
    CALL, CALL_BUILTIN, RETURN,
    BRANCH_ON_TRUE, BRANCH_ON_FALSE, BRANCH_UNCOND, 
    DEBUG_REG, DEBUG_SLOT, DEBUG_STACK,
    HALT
} Op;

#define INSTR_OPNAMES \
"add_int", "add_real",  "sub_int", "sub_real", \
"mul_int", "mul_real", "div_int", "div_real", \
"cmp_eq_int", "cmp_ne_int", "cmp_gt_int", "cmp_ge_int", \
"cmp_lt_int", "cmp_le_int", "cmp_eq_real", "cmp_ne_real", \
"cmp_gt_real", "cmp_ge_real", "cmp_lt_real", "cmp_le_real", \
"and", "or", "not", \
"int_to_real", "move", \
"push_stack_frame", "pop_stack_frame", \
"load", "store", "load_address", "load_indirect", "store_indirect", \
"int_const", "real_const", "string_const", \
"call", "call_builtin", "return", \
"branch_on_true", "branch_on_false", "branch_uncond", \
"debug_reg", "debug_slot", "debug_stack", \
"halt"

extern const char *instr_opnames[];

struct s_instr {
    Op      op;
    Arg     arg1;   
    Arg     arg2;
    Arg     arg3;
};

struct s_arg {
    AType   a_type;
    int     a_val;
    float   a_fltval;
};

struct s_param {
	int		d_lineno;
	PKind	d_kind;
	VType	d_type;
	char    *d_id;
    Instr   p_code;
};

struct s_params {
	Param   p_first;
	Params  p_rest;
};

struct s_decl {
    int         d_lineno;
	VType	    d_type;
    VarNames    d_varnames;
};

struct s_decls {
    Decl    d_first;
    Decls   d_rest;
};

struct s_varname {
    char    *v_id;
    Instr   v_code;
};

struct s_varnames {
    VarName     v_first;
    VarNames    v_rest;
};

typedef enum {
    STMT_ASSIGN, STMT_BLOCK, STMT_COND, STMT_READ, 
    STMT_SKIP, STMT_WHILE, STMT_WRITE, STMT_FOR, STMT_CALL
} SKind;

struct s_exprs {
	Expr	e_first;
	Exprs	e_rest;
};

typedef struct {
    char    *asg_id;
    Expr    asg_expr;
    Arg     stack_slot;
} Assign;

typedef struct {
    Expr    if_cond;
    Stmt    if_then;
    Stmt    if_else;
} Cond;

typedef struct {
    Expr    while_cond;
    Stmt    while_body;
} While;

typedef struct {
	char	*for_id;
	Expr	for_from_expr;
	Expr	for_to_expr;
	Stmt	for_body;
} For;

typedef struct {
	char	*call_id;
	Exprs	s_exprs;
} Call;

typedef union {
    Assign  s_assign;
    Stmts   s_block;
    Cond    s_cond;
    char    *s_read;
    Expr    s_skip;
    Expr    s_write;
    While   s_while;
	For		s_for;
	Call	s_call;
} SInfo;

struct s_stmt {
    int     s_lineno;
    SKind   s_kind;
    SInfo   s_info;
    Instr   s_code;
    SymbolTbl   *s_st;
};

struct s_stmts {
    Stmt    s_first;
    Stmts   s_rest;
};

struct s_prog {
    Procs   procs;
    SymbolTbl   *p_st;
};

struct s_header {
	char	*h_id;
	Params  h_params;
};

struct s_proc {
    int     p_main;				/* indicates a main() procedure */
    Header  p_header;
    Decls   p_decls;
    Stmts   p_body;
    int     p_param_ct;
    int     p_var_ct;
    SymbolTbl   *p_st;
};

struct s_procs {
	Proc	p_first;
	Procs	p_rest;
};

Instr alloc_code(int num_args);


#endif /* AST_H */

