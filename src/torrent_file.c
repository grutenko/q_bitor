#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#include "q_bitor/torrent_file.h"

bn_t *tr_decode_file(const char *path)
{
    FILE *f = fopen(path, "r");
    if(!f)
    {
        return NULL;
    }

    signed long int size;

    fseek(f, 0L, SEEK_END);
    size = ftell(f);
    rewind(f);

    char data[size + 1];

    fread(data, 1, size, f);
    data[size] = '\0';

    fclose(f);
    bn_t *bn = bn_decode(data);

    return bn;
}