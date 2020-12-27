#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *replace_str(char *str, char *orig, char *rep) {
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))
    return str;

  strncpy(buffer, str, p-str); 
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}

