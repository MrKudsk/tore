#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

int main(void)
{
  const char *home_path = getenv("HOME");
  if (home_path == NULL) {
    fprintf(stderr, "ERROR: No $HOME environment variable is setup. We need it to find the location of ~/.tore database.");
    return 1;
  }

    printf("HOME: is %s\n", home_path);

    return 0;
}
