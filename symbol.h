#ifndef SYMBOL_H
#define SYMBOL_H

#define INITIAL_TBL_SIZE 41
#define LOAD_LIMIT 0.85
#define TBL_MULT 2
#define HASH_C 300


#include "std.h"

/*
 * Table entry contains param/variable/proc name (key) and its type.
 */
typedef struct {
	char        *key;
    int         type;
    //int         array_size;
} TblEntry;

/*
 * Symbol table is a hash table containing info on functions and variables.
 */
typedef struct {
	int         table_size;
	int         num_items;
	TblEntry    *s_items;
} SymbolTbl;


/*
 * Returns an integer representing the position of the given word in 
 * the list. Returns -1 if it is not found
 */
extern int st_lookup(SymbolTbl *st, char *key);

/* 
 * Inserts a new entry into the hash table and returns the ht address. 
 * Uses "open hashing" (closed addressing) hash collision technique.
 */
extern int st_insert(SymbolTbl *st, char *key);

/* 
 * Create a table and return a pointer to it 
 */
extern SymbolTbl *st_init(size_t size);

/* Resize a table and return a pointer to it. */
extern SymbolTbl *st_rehash(SymbolTbl *st);

/* 
 * Dump the contents of the hash table to stdout. This is for debugging 
 * purposes.
 */
extern void st_dump(SymbolTbl *st);
extern void st_free(SymbolTbl *st);

extern int next_prime(int n); 
extern BOOL is_duplicate(int doc_ref, int *array, int size);

#endif /* SYMBOL_H */
