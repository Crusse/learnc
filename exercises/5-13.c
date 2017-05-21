#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>

#define LINE_LEN_INCREMENT 1024

/**
 * ./5-13 -42  prints the last 42 lines of stdin
 */
int main( int argc, char *argv[] ) {

  unsigned long limit = 10;

  if ( argc > 1 ) {

    if ( argc != 2 ) {
      printf( "Too many arguments\n" );
      return 1;
    }

    const char *limStr = argv[1];

    if ( limStr[0] != '-' ) {
      printf( "Invalid argument: %s\n", limStr );
      return 1;
    }

    for ( int i = 0; limStr[i] != '\0'; ++i ) {
      if ( !isdigit( limStr[i] ) ) {
        printf( "Argument does not contain a valid integer: %s\n", limStr );
        return 1;
      }
    }

    unsigned long argLimit = atol( limStr + 1 );
    if ( argLimit > 0 )
      limit = argLimit;
  }

  char *lines[ limit ];
  unsigned long lineCount = 0;
  char *line = (char*) malloc( LINE_LEN_INCREMENT );
  unsigned long lineLenMax = LINE_LEN_INCREMENT;
  unsigned long lineLen = 0;
  int c;

  while ( ( c = getchar() ) != EOF ) {

    if ( c == '\0' )
      continue;
    
    if ( c == '\n' ) {
      
      line[ lineLen ] = '\0';
      lines[ lineCount++ ] = line;

      free( line );
      line = (char*) malloc( LINE_LEN_INCREMENT );
      lineLenMax = LINE_LEN_INCREMENT;
      lineLen = 0;
    }
    else {

      if ( lineLen + 1 < lineLen ) {
        printf( "Exceeded maximum line length\n" );
        return 1;
      }
      
      if ( lineLen + 1 >= lineLenMax ) {
        lineLenMax = lineLen + LINE_LEN_INCREMENT;
        char *biggerLine = (char*) realloc( line, lineLenMax );
        if ( !biggerLine ) {
          printf( "Out of memory\n" );
          return 1;
        }
        line = biggerLine;
      }

      line[ lineLen++ ] = c;
    }
  }

  return 0;
}


