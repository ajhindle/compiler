#ifndef N7C_H
#define N7C_H

#include    "ast.h"
#include    "std.h"

extern  char     *f7file;          /* Name of file to parse */
extern  Program  parsed_program;

//static  void     report_error_and_exit(const char *msg);
void     report_error_and_exit(const char *msg);
extern  void     *checked_malloc(int num_bytes);

#endif /* N7C_H */

