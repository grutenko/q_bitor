
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "rpc/rpc.h"
#include "cwalk.h"
#include "commands.h"

#define ERR(m) fprintf(stderr, "ERR: %s\n", (m))

void c_add(int argc, char **argv)
{
    if(argc == 0)
    {
        ERR("add <torrent> <...>");
        exit(EXIT_FAILURE);
    }

    const char *path = argv[0];
    char cwd[ FILENAME_MAX ];
    char abs_path[ FILENAME_MAX ];

    if( NULL == getcwd(cwd, FILENAME_MAX) )
    {
        ERR("cwd error.");
        exit(EXIT_FAILURE);
    }

    cwk_path_get_absolute(cwd, path, abs_path, FILENAME_MAX);
    FILE *f = fopen(abs_path, "rb");

    if(f == NULL)
    {
        perror( "ERR: " );
        exit(EXIT_FAILURE);
    }

    
}