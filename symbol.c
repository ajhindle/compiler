/*
 * Username : ajhindle
 * Date     : 27-09-2006
 *
 * File     : hash_util.c
 *
 * This file contains a number of functions which relate to manipulating the 
 * hash table
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "symbol.h"
#include "util.h"




static int is_prime(int n); 

/*
** Return a pointer to a newly created hash table.
*/

/*
** Insert the specified key-value pair into the hash table.  
** Note that trailing newlines are stripped from both 'key' and 'value'.
** Copies of both strings are made for the hash table, so you are free to delete
** the originals after insertion.
*/

/*
** Return a pointer to the value associated with the specified key.
** Note that before lookup any trailing newlines are stripped from 'key'.
** Before returning this function sets the global variable 'number_of_lookups'
** to the number of lookups that were required in searching for 'key' in
** the hash table.
*/


/*
 * Return a pointer to a new hash table after resizing an old one.
*/ 

/*
 * return a hash for a given key
*/
static int hash(char *key, int tsize);

/*
 * add a doc ref to a table entry in a hash table
*/

/*
** Dump the contents of the hash table to stdout.
** This is for debugging purposes.
*/

/*
** Strip newlines from the end of a string.  Returns a 
** pointer to the string (this is useful if you want to use
** it in an expression).
**
** NOTE: this destructively updates the string so don't use
** it on strings that are stored in read-only memory.
*/

/*
** A safe string duplication function.  Aborts if memory
** cannot be allocated for the duplicate.
*/

/*
** Call malloc and abort if the specified amount of memory
** cannot be allocated.
*/


/*
void add_doc_ref(int doc_ref, Hash_Table *ht, int pos)
{
    int *temp;

    if ( isduplicate(doc_ref, ht->table[pos].doc_array, ht->table[pos].num_doc) )
        return;
    
    if (ht->table[pos].num_doc < ht->table[pos].array_size){
        ht->table[pos].doc_array[ht->table[pos].num_doc] = doc_ref;
    }
    else {
        ht->table[pos].array_size *= DOC_TBL_MULT;
        temp = calloc(ht->table[pos].array_size, sizeof(int));
        temp = memcpy(temp, ht->table[pos].doc_array, 
                sizeof(int)*ht->table[pos].num_doc);

        free(ht->table[pos].doc_array);
        ht->table[pos].doc_array = NULL;

        ht->table[pos].doc_array = temp;


        // fill rest of array with -1 
        memset(&ht->table[pos].doc_array[ht->table[pos].num_doc], -1, 
                sizeof(int)*(ht->table[pos].array_size - ht->table[pos].num_doc));

        ht->table[pos].doc_array[ht->table[pos].num_doc] = doc_ref;
    } 

    ht->table[pos].num_doc++ ;
}

*/

int is_duplicate(int doc_ref, int *array, int size) 
{
    int i;

    for (i = 0; i < size; i++) {
        if ( doc_ref == array[i] )
            return 1;
    }

    return 0;
}

SymbolTbl *st_init(size_t size)
{
	SymbolTbl *st;
    int i = 0;

	st = safe_malloc(sizeof(SymbolTbl));

	st->table_size = size;
	st->num_items = 0;

	st->s_item = safe_malloc(size * sizeof(TblEntry));

    for (i = 0; i < size; i++) {
        st->s_item[i].key = NULL; 
        /* st->table[i].num_doc = 0;
        st->table[i].array_size = DOC_TBL_SIZE;
        */
    }

	return st;

}

/*
int *make_doc_array(int size) 
{
    int *result;

    result = safe_malloc(sizeof(int)*size);

    memset(result, -1, sizeof(int)*size);

    return result;
}
*/

SymbolTbl *st_rehash(SymbolTbl *st)
{
    SymbolTbl *newht;
    int i = 0, newsize = 0, pos;

    newsize = next_prime(TBL_MULT * st->table_size);
    
    newht = st_init( newsize );


    for (i = 0; i < st->table_size; i++) {
        if ( st->s_item[i].key != NULL) {

            pos = st_insert(newht, chomp(st->s_item[i].key));
            /*
            newht->table[pos].num_doc = st->table[i].num_doc;
            newht->table[pos].array_size = st->table[i].array_size;
            
            newht->table[pos].doc_array = make_doc_array(ht->table[i].array_size);
            newht->table[pos].doc_array = memcpy(newht->table[pos].doc_array, 
                    ht->table[i].doc_array, 
                    sizeof(int)*ht->table[i].num_doc);
            */
        }
    }

    st_free(st);
    
    return newht;
}

void st_free(SymbolTbl *st)
{
    int i;

    for (i = 0; i < st->table_size; i++) {
        if (st->s_item[i].key != NULL) {
            // free(ht->table[i].doc_array);
            // ht->table[i].doc_array = NULL;
            free(st->s_item[i].key);
            st->s_item[i].key = NULL;
        }
    }
    free(st->s_item);
    st->s_item = NULL;
    free(st);
    st = NULL;
}

/* Determine whether argument is prime. */
int is_prime(int n) {
    int divisor;
    if (n<2) {
        return 0;
    }
    for (divisor=2; divisor*divisor<=n; divisor++) {
        if (n%divisor==0) {
            /* factor found, so can't be 
            * prime */
            return 0;
        }
    }
    /* no factors, so must be prime */
    return 1;
}


/* Find the next prime after n */
int next_prime(int n) {
    n = n+1;
    while (!is_prime(n)) {
        n = n+1;
    }
    return n;
}


static int hash(char *key, int tsize) 
{
    int h = 0, j, len;

    len = strlen(key);

    for (j = 0; j < len; j++) 
        h = (h * HASH_C + (int)key[j]) % tsize;

    return h;
}


int 
st_insert(SymbolTbl *st, char *key )
{
	int h = 0;

    h = hash(key, st->table_size);

    while (st->s_item[h].key != NULL) {
        h++;
        if (h >= st->table_size)
            h = 0;
    }

    st->s_item[h].key = safe_strdup(key);

    st->num_items++;  

    return h;
}


/*
 * Returns an integer representing the position of the sought after word in 
 * the array. Returns -1 if it is not found
*/

int 
st_lookup(SymbolTbl *st, char *key)
{
   int i = 0, h = 0;
   
   h = hash(key, st->table_size);
   i = h;
   for ( ; i < st->table_size; i++) {

       if (st->s_item[i].key == NULL) 
           return -1;

       if (strcmp(st->s_item[i].key, key) == 0) 
           return i;

       if ( i == h-1 )
           return -1;

       if ( i == st->table_size - 1 )
           i = -1;
   }

   return -1;

}

void
st_dump(SymbolTbl *st)
{
    int i, j;

    for (i = 0; i < st->table_size; i++) {

        if (st->s_item[i].key != NULL) {
            printf("\n%s,", st->s_item[i].key);
            //printf("%d,", st->table[i].num_doc);

            //for(j = 0; j < st->table[i].num_doc; j++)
            //    printf("%d,", ht->table[i].doc_array[j]);

        }
    
    }
    printf("\n%d", st->num_items);
    printf("\n%d\n", st->table_size);
}