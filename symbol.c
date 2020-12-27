/*
 * Username : ajh
 * Date     : 27-09-2006
 *            25-11-2020
 *
 * File     : symbol.c
 *
 * This file contains functions which relate to manipulating the 
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
** Return a pointer to the value associated with the specified key.
** Note that before lookup any trailing newlines are stripped from 'key'.
** Before returning this function sets the global variable 'number_of_lookups'
** to the number of lookups that were required in searching for 'key' in
** the hash table.
*/



/*
 * return a hash for a given key
*/
static int hash(char *key, int tsize);



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

int 
is_duplicate(int doc_ref, int *array, int size) {

    int i;

    for (i = 0; i < size; i++) {
        if ( doc_ref == array[i] )
            return 1;
    }

    return 0;
}

SymbolTbl 
*st_init(size_t size) {

	SymbolTbl *st;
    int i = 0;

	st = safe_malloc(sizeof(SymbolTbl));

	st->table_size = size;
	st->num_items = 0;

	st->s_items = safe_malloc(size * sizeof(TblEntry));

    for (i = 0; i < size; i++) {
        st->s_items[i].key = NULL; 
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

SymbolTbl 
*st_rehash(SymbolTbl *st) {

    SymbolTbl *newht;

    int i = 0, newsize = 0, pos;

    newsize = next_prime(TBL_MULT * st->table_size);
    
    newht = st_init( newsize );


    for (i = 0; i < st->table_size; i++) {
        if ( st->s_items[i].key != NULL) {

            pos = st_insert(newht, chomp(st->s_items[i].key));
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

void 
st_free(SymbolTbl *st) {
    int i;

    for (i = 0; i < st->table_size; i++) {
        if (st->s_items[i].key != NULL) {
            // free(ht->table[i].doc_array);
            // ht->table[i].doc_array = NULL;
            free(st->s_items[i].key);
            st->s_items[i].key = NULL;
        }
    }
    free(st->s_items);
    st->s_items = NULL;
    free(st);
    st = NULL;
}

/* Determine whether argument is prime. */
int 
is_prime(int n) {

    int divisor;
    if (n < 2) {
        return 0;
    }
    for (divisor =2 ; divisor * divisor <= n; divisor++) {
        if (n % divisor == 0) {
            /* factor found, so can't be prime */
            return 0;
        }
    }
    /* no factors, so must be prime */
    return 1;
}


/* Find the next prime after n */
int 
next_prime(int n) {

    n = n + 1;

    while (!is_prime(n)) 
        n = n + 1;

    return n;
}


int 
hash(char *key, int tsize) {

    int h = 0, j, len;

    len = strlen(key);

    for (j = 0; j < len; j++) 
        h = (h * HASH_C + (int)key[j]) % tsize;

    return h;
}


int 
st_insert(SymbolTbl *st, char *key) {

	int h = 0;

    h = hash(key, st->table_size);

    while (st->s_items[h].key != NULL) {
        h++;
        if (h >= st->table_size)
            h = 0;
    }

    st->s_items[h].key = safe_strdup(key);

    st->num_items++;  

    return h;
}


int 
st_lookup(SymbolTbl *st, char *key) {

   int i = 0, h = 0;
   
   h = hash(key, st->table_size);
   i = h;
   for ( ; i < st->table_size; i++) {

       if (st->s_items[i].key == NULL) 
           return -1;

       if (strcmp(st->s_items[i].key, key) == 0) 
           return i;

       if ( i == h - 1 )
           return -1;

       if ( i == st->table_size - 1 )
           i = -1;
   }

   return -1;

}

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
