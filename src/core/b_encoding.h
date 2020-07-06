#ifndef B_ENCODING_H
#define B_ENCODING_H

#include <stdbool.h>
#include <search.h>

enum BN_TYPE
{
    BN_INVALID,
    BN_INT32,
    BN_DOUBLE,
    BN_STRING,
    BN_LIST,
    BN_DICTIONARY
};

typedef struct bn_list_item
{
    enum BN_TYPE type;
    void* value;
    struct bn_list_item* next;
}
bn_list_item_t;

typedef struct bn_list
{
    bn_list_item_t* first;
    bn_list_item_t* last;
}
bn_list_t;

typedef struct bn_dict_item
{
    enum BN_TYPE type;

    void* value;
    char* key;

}
bn_dict_item_t;

typedef struct __posix_tnode {
	void			        *key;
	struct __posix_tnode	*llink, *rlink;
	signed char		        balance;
} bn_dict_t;

typedef struct
{
    enum BN_TYPE type;
    void* value;
}
bn_t;

typedef struct {
    char *string;
    long size;
} bstring_t;

bn_t* bn_create(enum BN_TYPE type, void* item);
bn_list_t* bn_create_list( void );
bn_dict_t* bn_create_dict( void );

void bn_add( bn_t* bn, enum BN_TYPE type, void* item);

int bn_add_to_list(bn_list_t* last_item, enum BN_TYPE type, void* item);
int bn_list_len(bn_list_t* list);


int bn_add_to_dict(bn_dict_t** dict, enum BN_TYPE type, const char* key, void* value);
bn_dict_item_t* bn_get_from_dict(bn_dict_t** dict, const char* key);
bool bn_dict_key_exist(bn_dict_t *dict, const char *key);

bn_list_item_t *bn_get_from_list(bn_list_t *list, unsigned int indx);
bn_dict_item_t *bn_get(bn_t *bn, const char *path);
bn_dict_item_t *bn_get_r(bn_dict_t *dict, const char *path);

void bn_free(bn_t* bn);
void bn_free_entity(enum BN_TYPE type, void* value);
void bn_free_list(bn_list_t* list);
void r_free_list_items(bn_list_item_t *list_item);
void bn_free_dict(bn_dict_t *dict);
void __deputy(void *dict);
void bn_free_bstring(bstring_t *bs);

bn_t *bn_decode_from_file(const char *file_path);
long int bn_decode_int32(char *s, char **p);
double bn_decode_double(char *s, char **p);
char *bn_decode_string(char *s, char **p);

bn_list_t *bn_decode_list(char *s, char **p);
bn_dict_t *bn_decode_dict(char *s, char **p);
void *bn_decode_entity(enum BN_TYPE type, char *s, char **p);
bn_t *bn_decode( char *s );

enum BN_TYPE bn_determine_type(char *s);
enum BN_TYPE bn_determine_number_type(char *s);

#endif // B_ENCODING_H