#include <stdio.h>

#define MAXLINE 1000
#define COLS 60

int main(int argc, char **argv) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;
  
  char whitespace[ MAXLINE ] = "";
  int whitespaceChars = 0;
  int seenChars = 0;
  int chr;
  
  while ( ( chr = getchar() ) != EOF ) {
    
    ++seenChars;
    
    if ( chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r' ) {
      
      whitespace[ whitespaceChars++ ] = chr;
    }
    else {
      
      for ( int i = 0; i < whitespaceChars; ++i )
        putchar( whitespace[ i ] );
      
      whitespaceChars = 0;
      
      putchar( chr );
    }
    
    if ( seenChars >= COLS || chr == '\n' || chr == '\r' || seenChars >= MAXLINE ) {
      putchar( ( chr == '\n' || chr == '\r' ) ? chr : '\n' );
      seenChars = 0;
      whitespaceChars = 0;
    }
  }
  
  return 0;
}

