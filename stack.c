/*
 * Author   : ajh
 * Date     : 24-12-2020
 *
 * File     : stack.c
 *
 * This module provides procedures (push, pop, etc) for operating on the 
 * stack.  The stack is useful for accessing information on the procedure in 
 * play 'right now', the last one, the one before, etc.
*/

#include <stdio.h>
#include "util.h"
#include "stack.h"


Stack
stack_init(void) {

	Stack   stack;

	stack = safe_malloc(sizeof(struct s_stack));

	stack->top = NULL;
	stack->num_of_frames = 0;

	return stack;
}


Frame
push(Stack stack) {

	Frame frame = safe_malloc(sizeof(struct s_frame));

	frame->next = stack->top;

	stack->top = frame;

    stack->num_of_frames++;

    return frame;
}


Frame
pop(Stack stack) {

	Frame frame = stack->top;

    if (stack->num_of_frames > 0) {
        stack->top = stack->top->next;
        stack->num_of_frames--;
    }

    return frame;
}

