#include <stdio.h>
#include <string.h>

#define NUM_CHARS_LENGTH (10 + ('z' - 'a' + 1))

void itob( int num, int base, char *retStr ) {
  
  int i = 0;
  
  while ( num ) {
    
    int remainder = num / base;
    
    if ( remainder > NUM_CHARS_LENGTH ) {
      // Base is too large
      retStr[ 0 ] = '\0';
      return;
    }
    else if ( remainder > 10 ) {
      retStr[ i++ ] = 'a' + ( remainder - 11 );
    }
    else {
      retStr[ i++ ] = 'a' + ( remainder - 11 );
    }
    
    num /= base;
  }
  
  retStr[ i ] = '\0';
}

int main(int argc, char **argv) {
  
  char str[ 1000 ] = "";
  int num = 128;
  
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
  
  itob( num, 64, str );
  printf( "%s\n", str );
}

