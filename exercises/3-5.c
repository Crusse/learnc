#include <stdio.h>
#include <string.h>
#include <limits.h>

#define NUM_CHARS_LENGTH (10 + ('z' - 'a' + 1))

void reverse( char str[] ) {
  
  int ch;
  int i, j;
  
  for ( i = 0, j = strlen( str ) - 1; i < j; ++i, --j ) {
    ch = str[ i ];
    str[ i ] = str[ j ];
    str[ j ] = ch;
  }
}

void itob( int num, int base, char *retStr ) {
  
  int i = 0;
  int sign = num;
  int carry = 0;

  // Base is too large
  if ( base < 2 || base > NUM_CHARS_LENGTH ) {
    retStr[ 0 ] = 'N';
    retStr[ 1 ] = 'a';
    retStr[ 2 ] = 'N';
    retStr[ 3 ] = '\0';
    return;
  }

  // Special case, as abs(INT_MIN) == INT_MAX + 1
  if ( num == INT_MIN ) {
    num += base;
    carry = 1;
  }

  if ( sign < 0 )
    num = -num;
  
  do {

    int remainder = num % base;
    
    if ( remainder >= 10 ) {
      retStr[ i++ ] = 'a' + ( remainder - 10 );
    }
    else {
      retStr[ i++ ] = '0' + remainder;
    }
    
    num /= base;

    if ( carry ) {
      num += 1;
      carry = 0;
    }
  }
  while ( num );

  if ( sign < 0 )
    retStr[ i++ ] = '-';
  
  retStr[ i ] = '\0';
  reverse( retStr );
}

int main(int argc, char **argv) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;
  
  char str[ 1000 ] = "";
  int num = 128;
  
  itob( 0, 2, str );
  printf( "%s\n", str );

  itob( -1, 2, str );
  printf( "%s\n", str );

  itob( 1, 2, str );
  printf( "%s\n", str );
  
  itob( num, 2, str );
  printf( "%s\n", str );
  
  itob( num, 8, str );
  printf( "%s\n", str );
  
  itob( num, 10, str );
  printf( "%s\n", str );
  
  itob( num, 16, str );
  printf( "%s\n", str );
  
  itob( num, 32, str );
  printf( "%s\n", str );

  itob( num, NUM_CHARS_LENGTH, str );
  printf( "%s\n", str );
  
  itob( num, 64, str );
  printf( "%s\n", str );

  printf( "\n" );

  itob( 15, 16, str );
  printf( "%s\n", str );
  itob( 256 * 256 * 256 - 1, 16, str );
  printf( "%s\n", str );
  itob( 100, 16, str );
  printf( "%s\n", str );

  printf( "\n" );
  
  itob( INT_MIN, 2, str );
  printf( "%s\n", str );
  itob( INT_MIN, 10, str );
  printf( "%s\n", str );
  itob( INT_MIN, 16, str );
  printf( "%s\n", str );

  printf( "\n" );
  
  itob( INT_MIN + 1, 2, str );
  printf( "%s\n", str );
  itob( INT_MIN + 1, 10, str );
  printf( "%s\n", str );
  itob( INT_MIN + 1, 16, str );
  printf( "%s\n", str );

  printf( "\n" );
  
  itob( INT_MAX, 2, str );
  printf( "%s\n", str );
  itob( INT_MAX, 10, str );
  printf( "%s\n", str );
  itob( INT_MAX, 16, str );
  printf( "%s\n", str );
}

