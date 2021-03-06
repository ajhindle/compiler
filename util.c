/*
 * Author   : ajhindle
 * Date     : 25-11-2020
 *
 * File     : util.c
 *
 * This module provides several utility functions.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "util.h" 

void
report_error_and_exit(const char *msg) {

    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

void 
*allocate(int size) {

    void    *addr;

    addr = malloc((size_t) size);
    if (addr == NULL) 
        report_error_and_exit("Out of memory");
    return addr;
}

void 
*safe_malloc(size_t size) {

	void *mem;

	if ((mem = malloc(size)) == NULL)
	{
		fprintf(stderr, "Cannot allocate memory.\n");
		exit(EXIT_FAILURE);
	}
	
	return mem;
}


/*
** Strip newlines from the end of a string.  Returns a 
** pointer to the string (this is useful if you want to use
** it in an expression).
**
** NOTE: this destructively updates the string so don't use
** it on strings that are stored in read-only memory.
*/
char 
*chomp(char *str) {

	size_t i;

	assert(str != NULL);
	
	for (i = strlen(str) - 1; str[i] == '\n'; i--)
		str[i] = '\0';	

	return str;
}


/*
** A safe string duplication function.  Aborts if memory
** cannot be allocated for the duplicate.
*/
char 
*safe_strdup(const char *str) {

	char *copy = NULL;

	assert(str != NULL);
	
	copy = safe_malloc((strlen(str) + 1) * sizeof(char));
	
	(void)strcpy(copy, str);

	return copy;	
}

FILE 
*safe_fopen(const char *path, const char *mode) {

    FILE *fp = NULL;

    assert(path != NULL);
    assert(mode != NULL);


    if ((fp = fopen(path, mode)) == NULL) {
        (void)fprintf(stderr, "Error while attempting to try open");
        (void)fprintf(stderr, " file %s: %s \n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    return fp;
}   
