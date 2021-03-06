/* n7c.c */

/*---------------------------------------------------------------------------
    A driver for a parser and pretty-printer for n7, for use in 
    433-361 asg 1, 2007.
    Harald Sondergaard, August 2007.
    ajhindle, July 2020.
---------------------------------------------------------------------------*/

#include    <string.h>
#include    <stdlib.h>
#include    "ast.h"
#include    "std.h"
#include    "traverse.h"
#include    "missing.h"
#include    "pretty.h"
#include    "analyser.h"
#include    "codegen.h"
#include    "symbol.h"

const char  *progname;
const char  *n7_infile;
Program     parsed_program = NULL;

extern FILE *yyin;

/*-------------------------------------------------------------------------*/
static  void  usage(void);
//void          report_error_and_exit(const char *msg);
void          *checked_malloc(int num_bytes);
/*-------------------------------------------------------------------------*/

int
main(int argc, char **argv) {

    const char  *in_filename;
    FILE        *fp = stdout;
    BOOL        pretty_print_only;

    progname = argv[0];
    pretty_print_only = FALSE;

    /* Process command line */

    if ((argc < 2) || (argc > 3)) {
        usage();
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
        in_filename = argv[1];

    if (argc == 3 && streq(argv[1],"-p")) {
        pretty_print_only = TRUE;
        in_filename = argv[2];
    }
    
    n7_infile = in_filename;
    yyin = fopen(in_filename, "r");
    if (yyin == NULL) {
        perror(in_filename);
        exit(EXIT_FAILURE);
    }

    if (yyparse() != 0) {
        /* The error message will already have been printed. */
        exit(EXIT_FAILURE);
    }

    if (pretty_print_only) 
        proc_prog(fp, pretty_prog, parsed_program);
    else {
        /* report_error_and_exit("Unable to generate code"); */
        proc_prog(fp, analyse_prog, parsed_program);
        proc_prog(fp, gen_prog, parsed_program);
    }

    return 0;
}

/*-------------------------------------------------------------------------*/

static void
usage(void) {

    printf("usage: n7c [-p] n7_source\n");
}

/*
void
report_error_and_exit(const char *msg) {

    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}


void *
checked_malloc(int num_bytes) {

    void    *addr;

    addr = malloc((size_t) num_bytes);
    if (addr == NULL) {
        report_error_and_exit("Out of memory");
    }

    return addr;
}
*/
/*-------------------------------------------------------------------------*/
