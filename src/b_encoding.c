#define _GNU_SOURCE
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "q_bitor/b_encoding.h"

/**
 * Добавляет в конец новый элемент.
 */
bn_t* bn_create(enum BN_TYPE type, void* item)
{
    bn_t* bn = malloc( sizeof(bn_t) );
    if(!bn)
    {
        return NULL;
    }
    bn->type = type;
    bn->value = item;

    return bn;
}

/** ------------------------------------------------------------------------------------------------------------
 *                                           LIST
 *  ------------------------------------------------------------------------------------------------------------ */

bn_list_t* bn_create_list( void )
{
    bn_list_t* list = malloc( sizeof(bn_t) );
    if(!list)
    {
        return NULL;
    }
    list->first = NULL;
    list->last = NULL;

    return list;
}

/**
 * Добавляет новый элемент и связывает его с указателем на последний элемент last_item.
 */
int bn_add_to_list(bn_list_t* list, enum BN_TYPE type, void* item)    
{
    bn_list_item_t* next_item = malloc(sizeof( bn_list_t ));
    if(!next_item)
    {
        return 0;
    }
    next_item->type = type;
    next_item->value = item;
    next_item->next = NULL;

    if(list->first == NULL || list->last == NULL)
    {
        list->first = next_item;
    }
    else
    {
        list->last->next = next_item;
    }
    list->last = next_item;

    return 1;
}

int bn_list_len(bn_list_t* list)
{
    if(!list)
    {
        return 0;
    }
    int count = 0;
    bn_list_item_t* cursor = list->first;

    while(cursor)
    {
        count++;
        cursor = cursor->next;
    }

    return count;
}

/** ------------------------------------------------------------------------------------------------------------
 *                                           DICTIONARY
 *  ------------------------------------------------------------------------------------------------------------ */

/**
 * Создает новый пустой словарь.
 */
bn_dict_t* bn_create_dict( void )
{
    bn_dict_t* dict = 0;
    return dict;
}

int __key_compare(const void* a, const void* b)
{
    return strcmp(((bn_dict_item_t *)a)->key, ((bn_dict_item_t *)b)->key);
}

int bn_add_to_dict(bn_dict_t** dict, enum BN_TYPE type, const char* key, void* value)
{
    bn_dict_item_t* item = malloc( sizeof(bn_dict_item_t) );
    if(!item)
    {
        return 0;
    }

    item->key   = strdup(key);
    item->value = value;
    item->type  = type;

    tsearch(item, (void **)dict, __key_compare);

    return 1;
}

bn_dict_item_t* bn_get_from_dict(bn_dict_t** dict, const char* key)
{
    bn_dict_item_t* find_a = malloc(sizeof(bn_dict_item_t));
    if(!find_a)
    {
        return NULL;
    }
    find_a->key = strdup(key);

    void *item = tfind(find_a, (void **)dict, __key_compare);
    free(find_a);

    return *(bn_dict_item_t **)item;
}

/** ------------------------------------------------------------------------------------------------------------
 *                                           FREE
 *  ------------------------------------------------------------------------------------------------------------ */

void bn_free(bn_t* bn)
{
    if(bn != NULL)
    {
        if(bn->value != NULL)
        {
            bn_free_entity(bn->type, bn->value);
        }
        free(bn);
    }
}

void bn_free_entity(enum BN_TYPE type, void* value)
{
    if(value == NULL)
    {
        return;
    }

    switch(type)
    {
        case BN_INT32       : free(value); break;
        case BN_DOUBLE      : free(value); break;
        case BN_STRING      : bn_free_bstring((bstring_t *)value); break;
        case BN_LIST        : bn_free_list((bn_list_t *)value); break;
        case BN_DICTIONARY  : bn_free_dict((bn_dict_t *)value); break;
    }
}

void bn_free_bstring(bstring_t *bs)
{
    if(bs->string != NULL)
    {
        free(bs->string);
    }
    free(bs);
}

void bn_free_list(bn_list_t* list)
{
    r_free_list_items(list->first);
    free(list);
}

void r_free_list_items(bn_list_item_t *list_item)
{
    if(list_item == NULL)
    {
        return;
    }
    if(list_item->next != NULL)
    {
        r_free_list_items(list_item->next);
    }
    bn_free_entity(list_item->type, list_item->value);
    free(list_item);
}

void bn_free_dict(bn_dict_t *dict)
{
    if(dict == NULL)
    {
        return;
    }

    if (dict->llink == NULL)
    {
        bn_free_dict(dict->llink);
    }
	if (dict->rlink == NULL)
    {
        bn_free_dict(dict->rlink);
    }

    bn_free_entity(((bn_dict_item_t *)dict)->type, ((bn_dict_item_t *)dict)->value); 
    free(dict);
}

/** ------------------------------------------------------------------------------------------------------------
 *                                           DECODE
 *  ------------------------------------------------------------------------------------------------------------ */

bn_t *bn_decode(char *s)
{
    enum BN_TYPE type = bn_determine_type(s);
    if(type == BN_INVALID)
    {
        fprintf(stderr, "%s (%c%c%c)\n", "Invalid type.", *s, *(s+1), *(s+2));
        return NULL;
    }
    
    char *p = s;
    void *value = bn_decode_entity(type, s, &p);
    if(value == NULL)
    {
        fprintf(stderr, "%s Offset: %ld\n", "Error parsing bencoding.", p-s);
        return NULL;
    }
    return bn_create(type, value);
}


enum BN_TYPE bn_determine_type(char *s)
{
    switch( *s )
    {
        case 'd'        : return BN_DICTIONARY; break;
        case 'l'        : return BN_LIST; break;
        case 'i'        : return bn_determine_number_type(s+1); break;
        case '0' ... '9': return BN_STRING; break;
        default: return BN_INVALID; break;
    }
}

enum BN_TYPE bn_determine_number_type(char *s)
{
    char *end = strstr(s, "e"),
         *point = strstr(s, ".");

    if(*s != '-' && (*s < (int)'0' || *s > (int)'9'))
    {
        return BN_INVALID;
    }

    return (point != NULL && point < end) ? BN_DOUBLE : BN_INT32;
}

long int bn_decode_int32(char *s, char **p)
{
    *p = strstr(s, "e") + 1;
    return strtol(s+1, NULL, 10);
}

double bn_decode_double(char *s, char **p)
{
    *p = strstr(s, "e") + 1;
    return strtod(s+1, NULL);
}

long bn_decode_string_length(char *s)
{
    char *e;
    return strtol(s, &e, 10);
}

char *bn_decode_string(char *s, char **p)
{
    char *e;
    long l = strtol(s, &e, 10);

    if(strstr(s, ":") != e)
    {
        fprintf(stderr, "%s line: %i %c\n", "Undefinded \":\". ", __LINE__, *e);
        return NULL;
    }

    char *d = calloc(l+1, sizeof(char));
    if(!d)
    {
        fprintf(stderr, "%s %li line: %i\n", "Cannot allocate:", (l+1)*sizeof(char), __LINE__);
        return NULL;
    }
    memcpy(d, e+1, l);
    d[l] = '\0';

    *p = e+l+1;

    return d;
}

bstring_t *bn_decode_bstring(char *s, char **p)
{
    long l = bn_decode_string_length(s);
    char *d = bn_decode_string(s, p);

    if(d == 0)
    {
        return NULL;
    }

    bstring_t *bs = (bstring_t *)malloc( sizeof(bstring_t) );
    bs->string = d;
    bs->size = l;

    return bs;
}

void *bn_decode_entity(enum BN_TYPE type, char *s, char **p)
{
    long int *i;
    double *d;
    
    switch( type )
    {
        case BN_INT32:
            i = malloc( sizeof(long int) );
            if(!i)
            {
                return NULL;
            }
            *i = bn_decode_int32(s, p);
            return (void *)i;
        break;
        case BN_DOUBLE: 
            d = malloc( sizeof(double) );
            if(!d)
            {
                return NULL;
            }
            *d = bn_decode_double(s, p);
            return (void *)d;
            break;
        case BN_STRING:
            return (void *)bn_decode_bstring(s, p);
            break;
        case BN_LIST:
            return (void *)bn_decode_list(s, p);
            break;
        case BN_DICTIONARY:
            return (void *)bn_decode_dict(s, p);
            break;
    }
}

bn_list_t *bn_decode_list(char *s, char **p)
{
    bn_list_t *list = bn_create_list();
    if(!list)
    {
        fprintf(stderr, "%s %i\n", "Cannot create list.", __LINE__);
        return NULL;
    }
    void *value;
    enum BN_TYPE type;

    // l####e
    //  ^
    *p = ++s;

    while( *s != 'e' && s[0] != '\0' )
    {
        type = bn_determine_type(s);
        if(type == BN_INVALID)
        {
            bn_free_list(list);
            fprintf(stderr, "%s %i\n", "Cannot parse type.", __LINE__);
            return NULL;
        }
        value = bn_decode_entity(type, s, p);
        if(value == NULL)
        {
            bn_free_list(list);
            fprintf(stderr, "%s - %i line: %i\n", "Error parse children value.", type, __LINE__);
            return NULL;
        }
        if(!bn_add_to_list(list, type, value))
        {
            bn_free_list(list);
            fprintf(stderr, "%s %i\n", "Cannot add to list.", __LINE__);
            return NULL;
        }
        if(s == *p)
        {
            bn_free_list(list);
            fprintf(stderr, "%s %i\n", "Unknown Error.", __LINE__);
            return NULL;
        }

        s = *p;
    }

    (*p)++;
    return list;
}

bn_dict_t *bn_decode_dict(char *s, char **p)
{
    bn_dict_t *dict = bn_create_dict();
    enum BN_TYPE type;
    char *key = NULL;
    void *value;

    // d####e
    //  ^
    *p = ++s;

    while( s[0] != 'e' && s[0] != '\0' )
    {
        type = bn_determine_type(s);
        if(type == BN_INVALID)
        {
            bn_free_dict(dict);
            fprintf(stderr, "%s %i\n", "Invalid type.", __LINE__);
            return NULL;
        }

        if(key == NULL)
        {
            if(type != BN_STRING)
            {
                bn_free_dict(dict);
                fprintf(stderr, "%s %i %i\n", "Key must be a string type. ", type, __LINE__);
                return NULL;
            }
            key = bn_decode_string(s, p);
            if(key == NULL)
            {
                bn_free_dict(dict);
                fprintf(stderr, "%s %i\n", "Key decode error.", __LINE__);
                return NULL;
            }
        }
        else
        {
            value = bn_decode_entity(type, s, p);
            if(value == NULL)
            {
                free(key);
                bn_free_dict(dict);
                fprintf(stderr, "%s %i\n", "Children is invalid.", __LINE__);
                return NULL;
            }
            if(!bn_add_to_dict(&dict, type, key, value))
            {
                free(key);
                bn_free_dict(dict);
                fprintf(stderr, "%s %i\n", "Cannot add to dict.", __LINE__);
                return NULL;
            }
            key = NULL;
        }

        if(s == *p)
        {
            bn_free_dict(dict);
            fprintf(stderr, "%s %i\n", "Unknown dict error.", __LINE__);
            return NULL;
        }

        s = *p;
    }

    (*p)++;
    return dict;
}