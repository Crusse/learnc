#include <stdio.h>
#include <limits.h>

void print_bits( unsigned char num ) {
  
  for ( int i = sizeof( num ) * CHAR_BIT - 1; i >= 0; --i ) {
    printf( "%d", ( num >> i ) & 1 );
  }
}

int bitcount( unsigned x ) {
  
  int b;
  
  for ( b = 0; x != 0; x >>= 1 ) {
    if ( x & 01 )
      b++;
  }
  
  return b;
}

int bitcount_faster( unsigned x ) {
  
  int i;
  
  for ( i = 0; x; ++i )
    x &= x - 1;
  
  return i;
}

int main(int argc, char **argv) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;
  
  printf( "32: " );
  print_bits( 32 );
  printf( "\n" );
  printf( "31: " );
  print_bits( 31 );
  printf( "\n" );
  printf( "30: " );
  print_bits( 30 );
  printf( "\n" );
  printf( "29: " );
  print_bits( 29 );
  printf( "\n\n" );
  
  printf( "Subtracting 1 from a number flips the rightmost 1-bit to be a 0-bit.\n\n" );

  printf( "30 & (30 - 1): " );
  print_bits( 30 & 29 );
  printf( "\n\n" );
  
  printf( "Bitcount before: %u\n", bitcount( 29 ) );
  printf( "Bitcount after:  %u\n", bitcount_faster( 29 ) );
  printf( "\n" );
  
  return 0;
}

