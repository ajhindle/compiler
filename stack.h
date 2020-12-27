
#ifndef STACK_H
#define STACK_H

#include "symbol.h"

typedef struct s_stack  *Stack;
typedef struct s_frame  *Frame;


struct s_stack {
	Frame       top;
    int         num_of_frames;
};


struct s_frame {
	SymbolTbl   *st;
    int         my_number;      //for testing
    Frame       next;
};



/*
 * Push a frame on to the stack.
 */
extern Frame push(Stack stack) ;

/* 
 * Pop a frame from the stack.
 */
extern Frame pop(Stack stack);

/* 
 * Create stack and return a pointer to it 
 */
extern Stack stack_init(void);

/* 
extern void st_dump(SymbolTbl *st);
extern void st_free(SymbolTbl *st);
*/

#endif /* STACK_H */
