#include "stddef.h"
#include <malloc.h>
#include "ceedling/unity.h"
#include "q_bitor/b_encoding.h"

void setUp(void)
{

}
void tearDown(void)
{

}

void test_create( void )
{
    bn_t* bn = bn_create(BN_STRING, "TestString");

    TEST_ASSERT_EQUAL_STRING("TestString", (char *)bn->value );
    TEST_ASSERT_EQUAL_INT(BN_STRING, bn->type);
}

void test_create_list( void )
{
    bn_list_t* bn = bn_create_list();
    TEST_ASSERT_NULL( bn->first );
    TEST_ASSERT_NULL( bn->last );
}

void test_create_dict( void )
{
    bn_dict_t* dict = bn_create_dict();
}

void test_add_number( void )
{
    bn_list_t* list = bn_create_list();
    int num = 25;

    bn_add_to_list(list, BN_INT32, &num);
    TEST_ASSERT_EQUAL_INT( num, *((int *)list->last->value) );
    TEST_ASSERT_EQUAL_INT(BN_INT32, list->last->type);
}

void test_list_add_string( void )
{
    bn_list_t* list = bn_create_list();

    char* string = "Test string";

    bn_add_to_list(list, BN_STRING, string);
    TEST_ASSERT_EQUAL_STRING(string, (char *)list->last->value );
    TEST_ASSERT_EQUAL_INT(BN_STRING, list->last->type);
}

void test_list_add_list( void )
{
    int num = 123;

    bn_list_t* list = bn_create_list();
    bn_add_to_list(list, BN_INT32, &num);

    bn_list_t* out_list = bn_create_list();
    bn_add_to_list(out_list, BN_LIST, list);

    TEST_ASSERT_EQUAL_INT( num, *((int *)((bn_list_t *)out_list->last->value)->last->value));
    TEST_ASSERT_EQUAL_INT( BN_LIST, out_list->last->type);
}

void test_list_len( void )
{
    char* str = "test string";
    bn_list_t* list = bn_create_list();

    TEST_ASSERT_EQUAL_INT(0, bn_list_len(list));
    
    bn_add_to_list(list, BN_STRING, str);

    TEST_ASSERT_EQUAL_INT(1, bn_list_len(list));

    bn_add_to_list(list, BN_STRING, str);
    TEST_ASSERT_EQUAL_INT(2, bn_list_len(list));
}

void test_dict_add( void )
{
    bn_dict_t* dict = bn_create_dict();

    int number = 24;

    bn_add_to_dict(&dict, BN_INT32, "test_item", &number);
}

void test_dict_get( void )
{
    bn_dict_t* dict = bn_create_dict();

    int number = 24;

    bn_add_to_dict(&dict, BN_INT32, "test_item", &number);
    bn_dict_item_t* item = bn_get_from_dict(&dict, "test_item");

    TEST_ASSERT_EQUAL_INT(number, *((int *)item->value) );
    TEST_ASSERT_EQUAL_INT(BN_INT32, item->type);
    TEST_ASSERT_FALSE(bn_dict_key_exist(dict, "undefined-key"));
    TEST_ASSERT_TRUE(bn_dict_key_exist(dict, "test_item"));
}

void test_get_r( void )
{
    bn_dict_t *dict = bn_create_dict();
    bn_dict_t *inner_dict = bn_create_dict();

    int number = 24;

    bn_add_to_dict(&inner_dict, BN_INT32, "test_item", &number);
    bn_add_to_dict(&dict, BN_DICTIONARY, "root", inner_dict);

    bn_t *bn = bn_create(BN_DICTIONARY, dict);

    bn_dict_item_t *item = bn_get(bn, "root.test_item");

    TEST_ASSERT_EQUAL_INT(BN_INT32, item->type);
    TEST_ASSERT_EQUAL_INT32(number, *(int *)item->value);
}


void test_free( void )
{
    bn_dict_t *dict = bn_create_dict(),
              *info = bn_create_dict(),
              *file = bn_create_dict();
    bn_list_t* files = bn_create_list();

    signed long long int len = -235235212435;
    char path[] = "/var/www/test",
         announce[] = "https://test.tracer.ru/tracer";

    bn_add_to_dict(&file, BN_INT32, "length", &len);
    bn_add_to_dict(&file, BN_STRING, "path", &path);
    bn_add_to_list(files, BN_DICTIONARY, (void *)file);
    bn_add_to_dict(&dict, BN_STRING, "announce", &announce);
    bn_add_to_dict(&info, BN_LIST, "files", (void *)files);
    bn_add_to_dict(&dict, BN_DICTIONARY, "info", (void *)info);

    bn_t* bn = bn_create(BN_DICTIONARY, (void *)dict);

    bn_free(bn);
}

void test_decode_number( void )
{
    char *p;
    TEST_ASSERT_EQUAL_INT32(2684354560000, bn_decode_int32("i2684354560000e", &p));
    TEST_ASSERT_EQUAL_FLOAT(2.05, bn_decode_double("i2.05e", &p));
    TEST_ASSERT_EQUAL_INT32(-2684354560000, bn_decode_int32("i-2684354560000e", &p));
    TEST_ASSERT_EQUAL_INT32(0, bn_decode_int32("0e4:test", &p));
    TEST_ASSERT_EQUAL_STRING("4:test", p);
}

void test_decode_string( void )
{
    char *p;
    TEST_ASSERT_EQUAL_STRING("1test", bn_decode_string("5:1testi3456e", &p));
    TEST_ASSERT_EQUAL_STRING("i3456e", p);
}

void test_determine_type( void )
{
    TEST_ASSERT_EQUAL_INT(BN_INVALID, bn_determine_type("irtge"));
    TEST_ASSERT_EQUAL_INT(BN_INT32, bn_determine_type("i25e"));
    TEST_ASSERT_EQUAL_INT(BN_DOUBLE, bn_determine_type("i2.5e"));
    TEST_ASSERT_EQUAL_INT(BN_LIST, bn_determine_type("li2.5ee"));
    TEST_ASSERT_EQUAL_INT(BN_STRING, bn_determine_type("4:test"));
    TEST_ASSERT_EQUAL_INT(BN_DICTIONARY, bn_determine_type("d4:testi2.5ee"));
}

void test_decode_list( void )
{
    char *p;
    char s[] = "l4:testi2ee";

    bn_list_t *list = bn_decode_entity(bn_determine_type(s), s, &p);

    TEST_ASSERT_NOT_NULL(list);

    TEST_ASSERT_EQUAL_INT(BN_STRING, list->first->type);
    TEST_ASSERT_EQUAL_STRING("test", *((char **)list->first->value));
    TEST_ASSERT_EQUAL_INT32(2L, *(long int *)list->first->next->value);
    TEST_ASSERT_EQUAL_INT(BN_INT32, list->first->next->type);
}


void test_decode_dict( void )
{
    char *p;
    char s[] = "d4:testi2ee";

    bn_dict_t *dict = bn_decode_entity(bn_determine_type(s), s, &p);

    TEST_ASSERT_NOT_NULL(dict);

    TEST_ASSERT_EQUAL_INT(BN_INT32, bn_get_from_dict(&dict, "test")->type);
    TEST_ASSERT_EQUAL_INT32(2L, *(long int *)bn_get_from_dict(&dict, "test")->value);
}

void test_decode_invalid( void )
{   
    TEST_ASSERT_NULL(bn_decode("invaliddata"));
    TEST_ASSERT_NULL(bn_decode("dgnvaliddata e"));
    TEST_ASSERT_NULL(bn_decode("l2invalid data e"));
    TEST_ASSERT_NULL(bn_decode("3463463g34h4h3h:3e"));
}