#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "5-18.h"

int main() {

  while (gettoken() != EOF) {
    strcpy(datatype, token);
    out[0] = '\0';
    dcl();
    if ( tokentype != '\n' )
      printf( "syntax error\n" );
    printf( "%s: %s %s\n", name, out, datatype );
  }
  return 0;
}

