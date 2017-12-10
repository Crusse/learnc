#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "5-18.h"

int main( int argc, char *argv[] ) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;

  while (gettoken() != EOF) {
    strcpy(datatype, token);
    out[0] = '\0';
    dcl();
    if ( tokentype != '\n' )
      printf( "syntax error\n" );
    if (name[0])
      printf( "%s: %s %s\n", name, out, datatype );
    resetdcl();
  }
  return 0;
}

