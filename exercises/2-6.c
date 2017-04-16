#include <stdio.h>
#include <limits.h>

unsigned char setbits( unsigned char toBits, int pos, int numBits, unsigned char fromBits ) {
  
  unsigned char leftShift = pos - numBits;
  unsigned char movedBits = ~( ~0 << numBits ) & fromBits;
  unsigned char mask = ~( ~0 << numBits ) << leftShift;
  unsigned char ret = toBits;

  // Clear area for moved bits
  ret = ret & ~mask;
  // Move bits
  ret = ret | ( movedBits << leftShift );

  return ret;
}

unsigned char invert( unsigned char bits, int pos, int numBits ) {
  
  unsigned char mask = ~( ~0 << numBits ) << pos - numBits;
  
  return bits ^ mask;
}

unsigned char rightrot( unsigned char bits, int numBits ) {
  return ( bits >> numBits ) | ( bits << ( sizeof( bits ) * CHAR_BIT - numBits ) );
}

void print_bits( unsigned char num ) {

  for ( int i = sizeof( num ) * CHAR_BIT - 1; i >= 0; --i ) {
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
  
  // Exercise 2-8
  
  printf( "Before: " );
  unsigned char bitsForRot = 6;
  print_bits( bitsForRot );
  printf( "\n" );
  bitsForRot = rightrot( bitsForRot, 2 );
  printf( "After:  " );
  print_bits( bitsForRot );
  printf( "\n\n" );
  
  return 0;
}

