#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define LOG_SQLITE3_ERROR(db) fprintf(stderr, "%s:%d: SQLITE3 ERROR: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db))

bool create_schema(sqlite3 *db)
{
  char *errmsg = NULL;
  const char *sql = 
    "CREATE TABLE IF NOT EXISTS Notifications (\n"
    "   id INTEGER PRIMARY KEY ASC,\n"
    "   title VARCHAR(256) NOT NULL,\n"
    "   created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\n"
    "   dismissed_at TIMESTAP DEFAULT NULL\n"
    ")\n";

  int ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (ret != SQLITE_OK) {
    LOG_SQLITE3_ERROR(db);
    return false;
  }

  return true;
}

bool show_notifications(sqlite3 *db)
{
  bool result = true; 
  sqlite3_stmt *stmt;

  int ret = sqlite3_prepare_v2(db, "SELECT id, title, created_at FROM Notifications WHERE dismissed_at IS NULL", -1, &stmt, NULL);
  if (ret != SQLITE_OK) {
    LOG_SQLITE3_ERROR(db);
    return_defer(false);
  }

  ret = sqlite3_step(stmt);
  for (int index = 0; ret == SQLITE_ROW; ++index) {
    int id = sqlite3_column_int(stmt, 0);
    const char *title = strdup((const char *)sqlite3_column_text(stmt, 1));
    const char *created_at = strdup((const char *)sqlite3_column_text(stmt, 2));
  
    printf("%d: %s, %s\n", id, title, created_at);
    ret = sqlite3_step(stmt);
  }
  
  if (ret != SQLITE_DONE) {
    LOG_SQLITE3_ERROR(db);
    return_defer(false);
  }
defer:
  if (stmt) sqlite3_finalize(stmt);
  return result;
}

bool dismiss_notification(sqlite3 *db, int id)
{
  bool result = true; 
  sqlite3_stmt *stmt = NULL;

  int ret = sqlite3_prepare_v2(db, "UPDATE Notifications SET dismissed_at = CURRENT_TIMESTAMP WHERE id = ?", -1, &stmt, NULL);
  if (ret != SQLITE_OK) {
    LOG_SQLITE3_ERROR(db);
    return_defer(false);
  }

  if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK) {
    LOG_SQLITE3_ERROR(db);
    return_defer(false);
  } 

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    LOG_SQLITE3_ERROR(db);
    return_defer(false);
  }

defer:
  if (stmt) sqlite3_finalize(stmt);
  return result;
}

int main(int argc, char **argv)
{
  int result = 0;
  // find home folder and then the database_path
  const char *program_name = shift(argv, argc);
  const char *home_path = getenv("HOME");
  if (home_path == NULL) {
    fprintf(stderr, "ERROR: No $HOME environment variable is setup. We need it to find the location of ~/.tore database.");
    return_defer(1);
  }

  const char *tore_path = nob_temp_sprintf("%s/.tore", home_path);
  printf("Database path: %s\n", tore_path);

  // Open database
  sqlite3 *db;

  int ret = sqlite3_open(tore_path, &db);
  if (ret != SQLITE_OK) {
    fprintf(stderr, "ERROR: %s, %s\n", tore_path, sqlite3_errstr(ret));
    return_defer(1);
  }
  printf("Succesfull opened %s\n", tore_path);

  if (!create_schema(db)) return_defer(1);

  if (argc <= 0) {
    if (!show_notifications(db)) return_defer(1);
  } else {
    const char *cmd = shift(argv, argc);
    if (strcmp(cmd, "dismiss") == 0) {
      if (argc <= 0) {
        fprintf(stderr, "ERROR: expected id\n");
        return_defer(1);
      }

      int id = atoi(shift(argv, argc));
      if (!dismiss_notification(db, id)) return_defer(1);
    }
  }

defer:
  if (db) sqlite3_close(db);
  return result;
}
