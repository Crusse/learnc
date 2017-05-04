#include <stdio.h>
#include <limits.h>
#include <string.h>

static int _itoa( int n, char str[], int magnitude, int startIndex ) {
  
  int carry = 0;
  
  // -INT_MIN == INT_MIN and abs(INT_MIN) == INT_MAX + 1, so we need to do some
  // trickery
  if ( n == INT_MIN ) {
    n += 10;
    carry = 1;
  }

  if ( n < 0 ) {
    n = -n;
    startIndex = 1;
    str[ 0 ] = '-';
  }

  int digit = n % 10 + '0';
  n /= 10;
  if ( carry )
    n += 1;
  
  int maxMagnitude = ++magnitude;

  if ( n ) {
    maxMagnitude = _itoa( n, str, magnitude, startIndex );
  }

  str[ startIndex + maxMagnitude - magnitude ] = digit;
  str[ startIndex + maxMagnitude ] = '\0';
    
  return maxMagnitude;
}

void itoa( int n, char str[] ) {
  _itoa( n, str, 0, 0 );
}

int main(int argc, char **argv) {
  
  char str[ 100 ] = "";

  itoa( 1239, str );
  printf( "itoa(%d) (0x%x):\n%s\n\n", 1239, 1239, str );
  
  itoa( -1239, str );
  printf( "itoa(%d) (0x%x):\n%s\n\n", -1239, -1239, str );
  
  itoa( 1, str );
  printf( "itoa(%d) (0x%x):\n%s\n\n", 1, 1, str );

  itoa( 0, str );
  printf( "itoa(%d) (0x%x):\n%s\n\n", 0, 0, str );

  itoa( -1, str );
  printf( "itoa(%d) (0x%x):\n%s\n\n", -1, -1, str );

  // INT_MAX is binary 0111...
  itoa( INT_MAX, str );
  printf( "itoa(INT_MAX) (0x%x):\n%s\n\n", INT_MAX, str );
  
  // INT_MIN is binary 1000...
  itoa( INT_MIN, str );
  printf( "itoa(INT_MIN) (0x%x):\n%s\n\n", INT_MIN, str );
}

