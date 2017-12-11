#include <stdio.h>
#include <string.h>

void escape( char to[], char from[] ) {
  
  int toI = 0;
  
  for ( int i = 0; from[ i ] != '\0'; ++i ) {
    
    switch ( from[ i ] ) {
      case '\t':
        to[ toI++ ] = '\\';
        to[ toI++ ] = 't';
        break;
      case '\r':
        to[ toI++ ] = '\\';
        to[ toI++ ] = 'r';
        break;
      case '\n':
        to[ toI++ ] = '\\';
        to[ toI++ ] = 'n';
        break;
      default:
        to[ toI++ ] = from[ i ];
        break;
    }
  }
  
  to[ toI ] = '\0';
}

void unescape( char to[], char from[] ) {
  
  int toI = 0;
  
  for ( int i = 0; from[ i ] != '\0'; ++i ) {
    
    switch ( from[ i ] ) {
      case '\\':
        switch ( from[ i + 1 ] ) {
          case 't':
            to[ toI++ ] = '\t';
            ++i;
            break;
          case 'r':
            to[ toI++ ] = '\r';
            ++i;
            break;
          case 'n':
            to[ toI++ ] = '\n';
            ++i;
            break;
          default:
            to[ toI++ ] = from[ i ];
            break;
        }
        break;
      default:
        to[ toI++ ] = from[ i ];
        break;
    }
  }
  
  to[ toI ] = '\0';
}

int main(int argc, char **argv) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;
  
  char from[] = "some\n\tte\\xt\nhere";
  // 'to' needs enough space for two characters per character, as each escaped
  // character takes up two characters' worth of space
  char escaped[ strlen( from ) * 2 + 1 ];
  char unescaped[ strlen( from ) * 2 + 1 ];

  escape( escaped, from );
  
  printf( "Escaped: \"%s\"\n", escaped );
  
  unescape( unescaped, escaped );
  
  printf( "Unescaped: \"%s\"\n", unescaped );

  return 0;
}

