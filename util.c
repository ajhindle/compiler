
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "util.h" 



void *safe_malloc(size_t size)
{
	void *mem;

	if ((mem = malloc(size)) == NULL)
	{
		fprintf(stderr, "Cannot allocate memory.\n");
		exit(EXIT_FAILURE);
	}
	
	return mem;
}

char *chomp(char *str)
{
	size_t i;
	assert(str != NULL);
	
	for (i = strlen(str) - 1; str[i] == '\n'; i--)
	{
		str[i] = '\0';	
	}

	return str;
}

char *safe_strdup(const char *str)
{
	char *copy = NULL;

	assert(str != NULL);
	
	copy = safe_malloc((strlen(str) + 1) * sizeof(char));
	
	(void)strcpy(copy, str);

	return copy;	
}

FILE *safe_fopen(const char *path, const char *mode)
{
    FILE *fp = NULL;

    assert(path != NULL);
    assert(mode != NULL);


    if ((fp = fopen(path, mode)) == NULL)
    {
        (void)fprintf(stderr, "Error while attempting to try open");
        (void)fprintf(stderr, " file %s: %s \n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    return fp;
}   
