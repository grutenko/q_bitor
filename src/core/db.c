#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <sqlite3.h>

#include "db.h"

#define CREATE_DB_SQL "PRAGMA foreign_keys = ON;\n" \
                      "CREATE TABLE IF NOT EXISTS distributions(\n" \
                      "     id           INTEGER PRIMARY KEY,\n" \
                      "     distrib_name TEXT NOT NULL,\n" \
                      "     piese_length INTEGER NOT NULL,\n" \
                      "     pieses       BLOB NOT NULL,\n" \
                      "     announce     TEXT NOT NULL,\n" \
                      "     title        TEXT NOT NULL,\n" \
                      "     many_files   BOOLEAN DEFAULT false\n" \
                      ");\n" \
                      "CREATE TABLE IF NOT EXISTS files(\n" \
                      "     id           INTEGER,\n" \
                      "     download_id  INTEGER,\n" \
                      "     file_size    INTEGER,\n" \
                      "     path         TEXT NOT NULL,\n" \
                      "     crc32        INTEGER DEFAULT NULL,\n" \
                      "     md5          TEXT DEFAULT NULL,\n" \
                      "     mtime        INTEGER DEFAULT NULL\n" \
                      ");\n"

bool db_create(sqlite3 *db)
{
    static void *r;
    char *errmsg;
    if( sqlite3_exec(db, CREATE_DB_SQL, NULL, r, &errmsg) != SQLITE_OK )
    {
        fprintf(stderr, "Couldn`t create database: %s\n", errmsg);
        return false;
    }

    return true;
}

sqlite3 *db_open( const char *path )
{
    static sqlite3 *db;

    if( sqlite3_open(path, &db) != SQLITE_OK )
    {
        fprintf(stderr, "%s\n", sqlite3_errmsg(db));
        return NULL;
    }

    return db;
}
