#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"


int main(void)
{
  int result = 0;
  // find home folder and then the database_path
  const char *home_path = getenv("HOME");
  if (home_path == NULL) {
    fprintf(stderr, "ERROR: No $HOME environment variable is setup. We need it to find the location of ~/.tore database.");
    return_defer(1);
  }

  const char *tore_path = nob_temp_sprintf("%s/.tore", home_path);
  printf("Database path: %s\n", tore_path);

  // Open database
  sqlite3 *db;
  // sqlite3_stmt *stmt;

  int ret = sqlite3_open(tore_path, &db);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "ERROR: %s, %s\n", tore_path, sqlite3_errstr(ret));
    return_defer(1);
  }
  printf("Succesfull opened %s\n", tore_path);

  char *errmsg = NULL;
  const char *sql = 
    "CREATE TABLE IF NOT EXISTS Notifications (\n"
    "   title VARCHAR(256) NOT NULL,\n"
    "   created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\n"
    "   dismissed_at TIMESTAP DEFAULT NULL\n"
    ")\n";
  ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "ERROR: could not create Notifications table: %s\n", errmsg);
    return_defer(1);
  }

defer:
  if (db) sqlite3_close(db);
  return result;
}
