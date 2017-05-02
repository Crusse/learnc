#include <stdio.h>
#include <limits.h>
#include <string.h>

void reverse( char str[] ) {
  
  int ch;
  int i, j;
  
  for ( i = 0, j = strlen( str ) - 1; i < j; ++i, --j ) {
    ch = str[ i ];
    str[ i ] = str[ j ];
    str[ j ] = ch;
  }
}

void itoa( int n, char str[] ) {
  
  int sign = n;
  int i = 0;
  
  // When we try to make 'n' positive, -INT_MIN loops around to be INT_MIN
  // again, when 'n' is an int. INT_MAX == -(INT_MIN + 1).
  // Note: unary plus doesn't seem to turn a negative integer into a positive
  // integer in C.
  printf( "// -(%d) = 0x%x\n", n, -n );
  // Taking one's complement of -n (i.e. INT_MIN) gives +n - 1 (i.e. INT_MAX)
  printf( "// ~(%d) = 0x%x (%d)\n", n, ~n, ~n );
  
  do {
    
    // Solution to exercise 3-4: we turn negative values into positive values
    // per digit in n, rather than turning the whole value of n into a positive
    // value at the start of the function. This ensures that INT_MIN gets
    // printed properly, because we always have enough space in 'digit' for any
    // complement of a negative value. On the other hand, if we turned n from
    // negative to positive at the start of the function, the complement of
    // INT_MIN would be INT_MIN, because in a two's complement system
    // abs(INT_MIN) == INT_MAX + 1.
    int digit = n % 10;
    if ( digit < 0 )
      digit = -digit;
    
    str[ i++ ] = digit + '0';
    n /= 10;
  }
  while ( n != 0 );
  
  if ( sign < 0 )
    str[ i++ ] = '-';
  
  str[ i ] = '\0';
  
  reverse( str );
}

int main(int argc, char **argv) {
  
  char str[ 100 ] = "";

  itoa( 1234, str );
  printf( "itoa(1234) (0x%x): %s\n\n", 1234, str );

  // INT_MAX is binary 0111...
  itoa( INT_MAX, str );
  printf( "itoa(INT_MAX) (0x%x): %s\n\n", INT_MAX, str );
  
  // INT_MIN is binary 1000...
  itoa( INT_MIN, str );
  printf( "itoa(INT_MIN) (0x%x): %s\n\n", INT_MIN, str );
}

