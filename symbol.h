#define INITIAL_TBL_SIZE 41
// #define MAX_STRING_SIZE 10000
#define LOAD_LIMIT 0.85
#define TBL_MULT 2
#define HASH_C 300
#define DOC_TBL_MULT 2
#define DOC_TBL_SIZE 10

typedef struct {
	char        *key;
    //int         num_doc;
    //int         array_size;
} TblEntry;

typedef struct {
	int         table_size;
	int         num_items;
	TblEntry    *s_item;
} SymbolTbl;


extern int st_lookup(SymbolTbl *st, char *key);
extern int st_insert(SymbolTbl *st, char *key);
extern void st_dump(SymbolTbl *st);
extern void st_free(SymbolTbl *st);
extern SymbolTbl *st_init(size_t size);
extern SymbolTbl *st_rehash(SymbolTbl *st);


extern int next_prime(int n); 
extern int is_duplicate(int doc_ref, int *array, int size);
/*
extern int *make_doc_array(int size);
extern void add_doc_ref(int doc_ref, SymbolTbl *st, int pos);
*/