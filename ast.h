/* ast.h */

/*---------------------------------------------------------------------------
    Definitions for the abstract syntax trees generated for n7 programs.
    For use in 433-361 asg 1, 2007.
    Harald Sondergaard, August 2007.
---------------------------------------------------------------------------*/

#ifndef AST_H
#define AST_H

typedef struct s_decl   	*Decl;
typedef struct s_decls 		*Decls;
typedef struct s_varname    *VarName;
typedef struct s_varnames 	*VarNames;
typedef struct s_param	    *Param;
typedef struct s_params	    *Params;
typedef struct s_expr   	*Expr;
typedef struct s_exprs		*Exprs;
typedef struct s_stmts  	*Stmts;
typedef struct s_stmt   	*Stmt;
typedef struct s_header		*Header;
typedef struct s_proc		*Proc;
typedef	struct s_procs		*Procs;
typedef struct s_prog   	*Program;

/* added BINOP_DIV */
typedef enum {
    BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV, 
	BINOP_EQ, BINOP_NE, BINOP_LT, BINOP_LE, BINOP_GT, BINOP_GE,
	BINOP_OR, BINOP_AND
} BinOp;

#define BINOP_NAMES "+", "-", "*", "/", "=", "!=", "<", "<=", ">", ">=", "and", "or"

extern const char *binopname[];

typedef enum {
    UNOP_MINUS, UNOP_NOT
} UnOp;

#define UNOP_NAMES "-", "not"

extern const char *unopname[];

typedef enum {
    EXPR_ID, EXPR_INTCONST, EXPR_FLTCONST, EXPR_BINOP, EXPR_UNOP
} EKind;

struct s_expr {
    int     e_lineno;
    EKind   e_kind;
    char    *e_id;      /* for ID */
    int     e_val;      /* for integer values */
	float	e_float;	/* for float values */
    UnOp    e_unop;     /* for UNOP */
    BinOp   e_binop;    /* for BINOP */
    Expr    e1;         /* for UNOP and BINOP */
    Expr    e2;         /* for BINOP */
};

typedef enum {
	VAL, REF, VALRES
} PKind;

typedef enum {
	INT, FLOAT
} VType;

struct s_param {
	int		d_lineno;
	PKind	d_kind;
	VType	d_type;
	char    *d_id;
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
};

struct s_stmts {
    Stmt    s_first;
    Stmts   s_rest;
};

struct s_prog {
    Procs   procs;
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
};

struct s_procs {
	Proc	p_first;
	Procs	p_rest;
};

#endif /* AST_H */

