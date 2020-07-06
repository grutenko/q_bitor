#define _GNU_SOURCE

#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h>
#include <fcntl.h>

#include "cwalk.h"
#include "db.h"
#include "ipc.h"
#include "config.h"
#include "loop.h"

#define UNIX_SOCK_PATH "/var/run/qbitor.unix"
#define SQLITE_DB_PATH "/var/lib/qbitor/db.sqlite3"

#define HEADER "QBitor Torrent client Daemon " PROJECT_VER " 2020 (c)\n" \
               "Alexey Fedorenko <alex@grutenko.ru>\n\n"

#define HELP "--unix           (-u)  <path>  Path to the UNIX Socket.     DEF: " UNIX_SOCK_PATH "\n" \
             "--database       (-d)  <path>  Path to the SQLite database. DEF: " SQLITE_DB_PATH "\n" \
             "--try-create-db                Try create database if not exists.\n" \
             "--fork           (-f)          Fork current current process and start as daemon.\n" \
             "--help           (-h)          Show help\n"

#define ERR(m) fprintf(stderr, "ERR: %s\n", (m))

struct opts {
    char *sock_path;
    char *database;
    bool try_create_db;
    bool fork;
};

void parse_opts(int argc, char **argv, struct opts *daemon_opts)
{
    int c, indx;

    struct option long_opts[] = {
        {"unix"         , required_argument, 0, 'u'},
        {"database"     , required_argument, 0, 'd'},
        {"try-create-db", no_argument      , 0, 't'},
        {"fork"         , no_argument      , 0, 'f'},
        {"help"         , no_argument      , 0, 'h'},
        {0              , 0                , 0, 0}
    };
    char cwd[ FILENAME_MAX ];
    char abs_path[ FILENAME_MAX ];

    while( (c = getopt_long(argc, argv, "u:d:fh", long_opts, &indx)) != -1 )
    {
        switch(c)
        {
            case 'u':
            {
                if(optarg == NULL)
                {
                    ERR("Invalid value for --unix (-u). \nCheck --help for details.");
                    exit(EXIT_FAILURE);
                }
                if( NULL == getcwd(cwd, FILENAME_MAX) )
                {
                    ERR("cwd error.");
                    exit(EXIT_FAILURE);
                }
                cwk_path_get_absolute(cwd, strdup(optarg), abs_path, FILENAME_MAX);
                daemon_opts->sock_path = strdup(abs_path);
                break;
            }
            case 'd':
            {
                if(optarg == NULL)
                {
                    ERR("Invalid value for --database (-d). \nCheck --help for details.");
                    exit(EXIT_FAILURE);
                }
                if( NULL == getcwd(cwd, FILENAME_MAX) )
                {
                    ERR("cwd error.");
                    exit(EXIT_FAILURE);
                }
                cwk_path_get_absolute(cwd, strdup(optarg), abs_path, FILENAME_MAX);
                daemon_opts->database = strdup(abs_path);
                break;
            }
            case 't':
            {
                daemon_opts->try_create_db = true;
                break;
            }
            case 'f':
            {
                daemon_opts->fork = true;
                break;
            }
            case 'h':
            {
                printf(HELP);
                exit(EXIT_SUCCESS);
                break;
            }
            default:
            {
                fprintf(stderr, "qbitor --help for details.\n");
                exit(EXIT_FAILURE);
                break;
            }
        }
    }
}

bool file_exist(const char *path)
{
    int f = open(path, O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR);
    if(f < 0)
    {
        return errno == EEXIST;
    }
    else
    {
        close(f);
        unlink(path);
    }
}

int main(int argc, char **argv)
{
    printf(HEADER);
    struct opts daemon_opts = {
        .sock_path      = UNIX_SOCK_PATH,
        .database       = SQLITE_DB_PATH,
        .try_create_db  = false,
        .fork           = false
    };

    parse_opts(argc, argv, &daemon_opts);

    printf("Create UNIX socket...\n");
    int fd = ipc_open_unix(daemon_opts.sock_path);
    if(fd == -1)
    {
        fprintf(stderr, "%s %s: %s\n",
            "ERR: Can`t create new UNIX socket in",
            daemon_opts.sock_path,
            strerror(errno)
        );
        exit(EXIT_FAILURE);
    }

    sqlite3 *db;
    if( !file_exist(daemon_opts.database) )
    {
        if(daemon_opts.try_create_db)
        {
            db = db_open(daemon_opts.database);
            printf("Create database %s...\n", daemon_opts.database);
            if( !db_create(db) )
            {
                unlink(daemon_opts.database);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            fprintf(stderr, "Database file not found. %s\n", daemon_opts.database);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        db = db_open(daemon_opts.database);
        if(db == NULL)
        {
            exit(EXIT_FAILURE);
        }
    }
    
    __start(fd, db);

    if( close(fd) == -1)
    {
        perror("Failure close socket:");
        exit(EXIT_FAILURE);
    }
    else
    {
        unlink(daemon_opts.sock_path);
    }
    

    return 0;
}