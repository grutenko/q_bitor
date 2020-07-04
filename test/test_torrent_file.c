#include <ceedling/unity.h>
#include <malloc.h>
#include <q_bitor/b_encoding.h>
#include <q_bitor/torrent_file.h>

void setUp(void)
{

}
void tearDown(void)
{

}

void test_decode_from_file( void )
{
    bn_t* bn = tr_decode_file("/mnt/c/Users/Huawei/Downloads/[HorribleSubs] Vinland Saga (01-24) [1080p] (Unofficial Batch).torrent");
    
    TEST_ASSERT_NOT_NULL(bn);
    TEST_ASSERT_EQUAL_INT(BN_DICTIONARY, bn->type);
    TEST_ASSERT_NOT_NULL(bn->value);

    bn_dict_item_t *files_item = bn_get(bn, "info.files");

    TEST_ASSERT_NOT_NULL(files_item);
    TEST_ASSERT_NOT_NULL(files_item->value);
    TEST_ASSERT_EQUAL_INT(BN_LIST, files_item->type);
}