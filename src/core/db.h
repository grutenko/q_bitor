#include <sqlite3.h>

bool db_create(sqlite3 *db);
sqlite3 *db_open( const char *path );