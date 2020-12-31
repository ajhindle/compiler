/*
 * Author   : ajhindle
 * Date     : 27-09-2006
 *            25-11-2020
 *
 * File     : symbol.c
 *
 * This module contains functions which operate on the symbol table. 
 * The symbol table is used for (quickly) looking up info on 
 * variables, procedures, etc.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>


#include "symbol.h"
#include "util.h"

static BOOL is_prime(int n); 
static int hash(char *key, int tsize);


BOOL
is_duplicate(int doc_ref, int *array, int size) {

    int i;

    for (i = 0; i < size; i++) {
        if ( doc_ref == array[i] )
            return TRUE;
    }

    return FALSE;
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


SymbolTbl 
*st_rehash(SymbolTbl *st) {

    SymbolTbl *newht;

    int i = 0, newsize = 0, pos = -1;

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
            free(st->s_items[i].key);
            st->s_items[i].key = NULL;
        }
    }
    free(st->s_items);
    st->s_items = NULL;
    free(st);
    st = NULL;
}


/* 
 * Determine whether given number is prime. 
 */
BOOL
is_prime(int n) {

    int divisor;
    if (n < 2) {
        return FALSE;
    }
    for (divisor =2 ; divisor * divisor <= n; divisor++) {
        if (n % divisor == 0) {
            /* factor found, so can't be prime */
            return FALSE;
        }
    }
    /* no factors, so must be prime */
    return TRUE;
}


/* 
 * Find the next prime after n 
 */
int 
next_prime(int n) {

    n = n + 1;

    while (!is_prime(n)) 
        n = n + 1;

    return n;
}


/*
 * return a hash for a given key
*/
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
        }
    
    }
    printf("Number of items: %d\n", st->num_items);
    printf("Size of table: %d\n", st->table_size);
}
