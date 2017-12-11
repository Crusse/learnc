#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#define LINEPTRS_INCREMENT 64
#define LINE_LEN_INCREMENT 128

// Try changing this to unsigned char, and pass a large file, or a file with
// long lines into stdin. This program will handle the resulting space problem
// gracefully.
typedef unsigned long tail_limit_t;

/**
 * ./5-13 -42  prints the last 42 lines of stdin
 */
int main( int argc, char *argv[] ) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;

  tail_limit_t limit = 10;

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

    for ( int i = 1; limStr[i] != '\0'; ++i ) {
      if ( !isdigit( limStr[i] ) ) {
        printf( "Argument does not contain a valid integer: %s\n", limStr );
        return 1;
      }
    }

    tail_limit_t argLimit = atoll( limStr + 1 );
    if ( argLimit > 0 )
      limit = argLimit;
  }

  size_t ptrArrLen = ( limit <= LINEPTRS_INCREMENT )
    ? limit
    : LINEPTRS_INCREMENT;

  char **lines = (char**) malloc( ptrArrLen * sizeof( char* ) );
  
  if ( !lines ) {
    printf( "Out of memory\n" );
    return 1;
  }

  // When we push a new line and pop the oldest line, we don't want to shift
  // all array values around. Instead we use this value to denote the array
  // index where the first printed line starts.
  tail_limit_t firstLineIndex = 0;
  tail_limit_t lineCount = 0;
  unsigned long long seenLineCount = 0;
  
  tail_limit_t lineLenMax = LINE_LEN_INCREMENT;
  char *line = (char*) malloc( lineLenMax * sizeof( char ) );
  tail_limit_t lineLen = 0;
  int c;
  
  if ( !line ) {
    printf( "Out of memory\n" );
    return 1;
  }

  while ( 1 ) {

    c = getchar();

    // A NULL byte would kill our output, so we ignore it
    if ( c == '\0' )
      continue;
    
    if ( c == '\n' || c == EOF ) {
      
      ++seenLineCount;
      
      line[ lineLen ] = '\0';
      
      if ( lineCount < limit ) {
      
        lines[ lineCount++ ] = line;

        assert( lineCount <= ptrArrLen );

        if ( lineCount == ptrArrLen ) {
          
          // Reached the limits of our chosen integer type for lineLenMax
          if ( (tail_limit_t)( ptrArrLen + LINEPTRS_INCREMENT ) <= ptrArrLen ) {
            printf( "Exceeded maximum line count\n" );
            return 1;
          }
          
          ptrArrLen += LINEPTRS_INCREMENT;
          
          char **biggerLineArr = (char**) realloc( lines, ptrArrLen * sizeof( char* ) );
          if ( !biggerLineArr ) {
            printf( "Out of memory while allocating for line %llu in line list\n", seenLineCount );
            return 1;
          }
          lines = biggerLineArr;
        }
      }
      // We already have the max amount of lines. Get rid of the oldest line.
      else {
        
        free( lines[ firstLineIndex ] );
        lines[ firstLineIndex ] = line;
        firstLineIndex = ( firstLineIndex + 1 ) % limit;
      }
      
      if ( c == EOF )
        break;
      
      lineLenMax = LINE_LEN_INCREMENT;
      line = (char*) malloc( lineLenMax * sizeof( char ) );
      lineLen = 0;
    }
    else {

      // Reached the limits of our chosen integer type for lineLen
      if ( lineLen + 1 < lineLen ) {
        printf( "Exceeded maximum line length\n" );
        return 1;
      }

      line[ lineLen++ ] = c;
      
      assert( lineLen <= lineLenMax );

      if ( lineLen == lineLenMax ) {
        
        // Reached the limits of our chosen integer type for lineLenMax
        if ( (tail_limit_t)( lineLenMax + LINE_LEN_INCREMENT ) <= lineLenMax ) {
          printf( "Exceeded maximum line length\n" );
          return 1;
        }
        
        lineLenMax += LINE_LEN_INCREMENT;
        
        char *biggerLine = (char*) realloc( line, lineLenMax * sizeof( char ) );
        if ( !biggerLine ) {
          printf( "Out of memory while allocating for characters on line %llu\n", seenLineCount + 1 );
          return 1;
        }
        line = biggerLine;
      }
    }
  }

  tail_limit_t i = firstLineIndex;
  tail_limit_t printed = 0;
  for ( ; printed < lineCount; ++printed, ++i ) {
    printf( "%4lu %s\n", printed + 1, lines[ i % lineCount ] );
  }

  return 0;
}
