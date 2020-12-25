/*
 * Username : ajh
 * Date     : 24-12-2020
 *
 * File     : stack.c
 *
 * This file contains functions for operating on the stack.
*/


#include <stdio.h>

#include "util.h"
#include "stack.h"




Stack
stack_init(void) {

	Stack   stack;

	stack = safe_malloc(sizeof(Stack));

	stack->top = NULL;
	stack->num_of_frames = 0;

	return stack;
}


Frame
push(Stack stack) {

	Frame frame = safe_malloc(sizeof(Frame));

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

/*
void
st_dump(SymbolTbl *st)
{
    int i, h;
            
    printf("i : h : key\n");

    for (i = 0; i < st->table_size; i++) {

        if (st->s_items[i].key != NULL) {
            h = hash( st->s_items[i].key , st->table_size);
            printf("%d : %d : %s\n", i, h, st->s_items[i].key);
            //printf("%d,", st->table[i].num_doc);

            //for(j = 0; j < st->table[i].num_doc; j++)
            //    printf("%d,", ht->table[i].doc_array[j]);

        }
    
    }
    printf("number of items: %d\n", st->num_items);
    printf("size of table: %d\n", st->table_size);
}

*/
