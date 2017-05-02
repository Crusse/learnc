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
  // Note: unary plus doesn't turn a negative integer into a positive
  // integer in C (nor in JS or PHP).
  printf( "// -(%d) = 0x%x\n", n, -n );
  // Taking one's complement of -n (i.e. INT_MIN) gives +n - 1 (i.e. INT_MAX)
  printf( "// ~(%d) = 0x%x (%d)\n", n, ~n, ~n );

  // We can't turn INT_MIN to -INT_MIN, because -INT_MIN == INT_MIN in a two's
  // complement system. INT_MAX has space for one less non-zero values than INT_MIN,
  // because abs(INT_MIN) == INT_MAX + 1.
  if ( n == INT_MIN ) {
    // Example: n = -210
    n += 10; // -209
    n = -n;  // 209
    str[ i++ ] = n % 10 + '0';
    n /= 10; // 20
    n += 1; // 21
  }
  else {
    // In C89 negative integers will truncate to implementation-dependent
    // direction, e.g. -19/10 will be -1 or -2. That's why we must make sure
    // that n is a positive integer before dividing. In C99 it's always
    // truncated towards zero.
    if ( sign < 0 )
      n = -n;
    str[ i++ ] = n % 10 + '0';
    n /= 10;
  }
  
  while ( n != 0 ) {
    str[ i++ ] = n % 10 + '0';
    n /= 10;
  }
  
  if ( sign < 0 )
    str[ i++ ] = '-';
  
  str[ i ] = '\0';
  
  reverse( str );
}

int main(int argc, char **argv) {
  
  char str[ 100 ] = "";

  itoa( 1239, str );
  printf( "itoa(1239) (0x%x): %s\n\n", 1239, str );

  itoa( 0, str );
  printf( "itoa(0) (0x%x): %s\n\n", 0, str );

  itoa( -1, str );
  printf( "itoa(-1) (0x%x): %s\n\n", -1, str );

  // INT_MAX is binary 0111...
  itoa( INT_MAX, str );
  printf( "itoa(INT_MAX) (0x%x): %s\n\n", INT_MAX, str );
  
  // INT_MIN is binary 1000...
  itoa( INT_MIN, str );
  printf( "itoa(INT_MIN) (0x%x): %s\n\n", INT_MIN, str );
}

