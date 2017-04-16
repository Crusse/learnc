#include <stdio.h>

unsigned char setbits( unsigned char toBits, int pos, int numBits, unsigned char fromBits ) {
  
  unsigned char movedBits = ~( ~0 << numBits ) & fromBits;
  unsigned char mask = ~( ~0 << numBits ) << pos - numBits;
  unsigned char ret = toBits;

  // Clear area for moved bits
  ret = ret & ~mask;
  // Move bits
  ret = ret | ( movedBits << pos - numBits );

  return ret;
}

unsigned char invert( unsigned char bits, int pos, int numBits ) {
  
  unsigned char mask = ~( ~0 << numBits ) << pos - numBits;
  
  return bits ^ mask;
}

void print_bits( unsigned char num ) {

  for ( int i = sizeof( num ) * 8; i >= 0; --i ) {
    printf( "%d", ( num >> i ) & 1 );
  }
}

int main(int argc, char **argv) {
  
  unsigned char bits1 = 15;
  unsigned char bits2 = 22;
  
  // Exercise 2-6
  
  printf( "Before:\n" );
  printf( "Bits 1: " );
  print_bits( bits1 );
  printf( "\n" );
  printf( "Bits 2: " );
  print_bits( bits2 );
  printf( "\n" );

  bits2 = setbits( bits2, 4, 2, bits1 );

  printf( "After:\n" );
  printf( "Bits 1: " );
  print_bits( bits1 );
  printf( "\n" );
  printf( "Bits 2: " );
  print_bits( bits2 );
  printf( "\n\n" );
  
  // Exercise 2-7
  
  printf( "Before: " );
  unsigned char bits = 22;
  print_bits( bits );
  printf( "\n" );
  bits = invert( bits, 4, 2 );
  printf( "After:  " );
  print_bits( bits );
  printf( "\n\n" );
  
  return 0;
}

