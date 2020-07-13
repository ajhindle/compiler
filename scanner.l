/* scanner.l */

/*---------------------------------------------------------------------------
   A flex spec for a small language, n7, for use in 433-361 asg 1, 2007.  
   Harald Sondergaard, August 2007.
---------------------------------------------------------------------------*/

NUM     [0-9]+
ID      [a-zA-Z_][A-Za-z_0-9]*
nl      [\n\f]
nonl    [^\n\f]

%{

#include <stdlib.h>
#include <string.h>
#include "std.h"
#include "ast.h"
#include "parser.h"

#define YY_NO_UNPUT

int yylex(void);
int yywrap(void);

extern int ln;

%}

%%

do              { return DO_TOKEN; }
else            { return ELSE_TOKEN; }
if              { return IF_TOKEN; }
read            { return READ_TOKEN; }
skip            { return SKIP_TOKEN; }
then            { return THEN_TOKEN; }
int             { return INT_TOKEN; }
float           { return FLOAT_TOKEN; }
while           { return WHILE_TOKEN; }
write           { return WRITE_TOKEN; }
proc            { return PROC_TOKEN; }
end             { return END_TOKEN; }
ref             { return REF_TOKEN; }
val             { return VAL_TOKEN; }
valres          { return VALRES_TOKEN; }

{NUM}           { yylval.int_val = atoi(yytext); return INT_VAL_TOKEN; }
{NUM}"."{NUM}   { yylval.float_val = strtod(yytext,NULL); return FLT_VAL_TOKEN; }

{ID}            { yylval.str_val = (char *) strdup(yytext); return IDENT_TOKEN; }

[-+*,;(){}]     { return yytext[0]; }         /* single character operators */

":="            { return ASSIGN_TOKEN; }

"#"{nonl}*{nl}  { ln++; }

{nl}            { ln++; }

[ \t]+          ; /* skip whitespace */

.               { return INVALID_TOKEN; }     /* will cause a syntax error */

%%

int yywrap() {
    return 1;
}

/*-------------------------------------------------------------------------*/

