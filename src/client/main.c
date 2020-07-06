#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commands.h"
#include "config.h"

#define HEADER "QBitor Torrent client " PROJECT_VER " 2020 (c)\n" \
               "Alexey Fedorenko <alex@grutenko.ru>\n\n"

#define CMD_AVAILABLE "available: add <torrent_file> <...>\n"

#define ERR(m) fprintf(stderr, "ERR: %s\n", (m))
#define ERR_CMD(m, c) fprintf(stderr, "ERR: %s %s\n"CMD_AVAILABLE"\n", (m), (c))

#define CMD_ARGC(argc) (argc)-2
#define CMD_ARGV(argv)  (char **)((unsigned long)(argv) + (sizeof(char *)) * 2)
#define EQ(e, a) strcmp((e), (a)) == 0

int main(int argc, const char **argv)
{
    printf(HEADER);

    if( argc < 2 )
    {
        ERR("<cmd> <args...>");
        exit(EXIT_FAILURE);
    }

    if( EQ("add", argv[1]) ) c_add(CMD_ARGC(argc), CMD_ARGV(argv));
    else
    {
        ERR_CMD("Command not found:", argv[1]);
        exit(EXIT_FAILURE);
    }

    return 0;
}