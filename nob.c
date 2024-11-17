#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_FOLDER "./build/"

bool build_sqlite3(Nob_Cmd *cmd)
{
  const char *output_path = BUILD_FOLDER"sqlite3.o";
  const char *input_path = "sqlite-amalgamation-3470000/sqlite3.c"; 
  int rebuild_is_needed = nob_needs_rebuild1(output_path, input_path);
  if (rebuild_is_needed < 0) return 1;
  if (rebuild_is_needed) {
    nob_cmd_append(cmd, "cc", "-DSQLITE_OMIT_LOAD_EXTENSION", "-O3", "-o", output_path, "-c", input_path);
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
  } else {
    nob_log(NOB_INFO, "%s is up to date", output_path);
  }
  return true;
}


int main(int argc, char *argv[])
{
  NOB_GO_REBUILD_URSELF(argc, argv);

  if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;
  
  Nob_Cmd cmd = {0};

  if (!build_sqlite3(&cmd)) return 1;
  nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-Wswitch-enum", "-ggdb", "-static", "-I./sqlite-amalgamation-3470000/", "-o", BUILD_FOLDER"tore", "tore.c", BUILD_FOLDER"sqlite3.o");
  if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;
  return EXIT_SUCCESS;
}
