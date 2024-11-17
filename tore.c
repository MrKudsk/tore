#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

#define NOB_IMPLEMENTATION
#include "nob.h"


int main(void)
{
  const char *home_path = getenv("HOME");
  if (home_path == NULL) {
    fprintf(stderr, "ERROR: No $HOME environment variable is setup. We need it to find the location of ~/.tore database.");
    return 1;
  }

  const char *tore_path = nob_temp_sprintf("%s/.tore", home_path);
  printf("Database path: %s\n", tore_path);

  sqlite3 *db;
  sqlite3_stmt *stmt;

  sqlite3_open(tore_path, &db);
  if (db == NULL) {
    fprintf(stderr, "ERROR: could not open %s\n", tore_path);
    return 1;
  }

  printf("Succesfull opened %s\n", tore_path);

  sqlite3_close(db);

  return 0;
}
