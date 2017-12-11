#include <limits.h>
#include <float.h>
#include <stdio.h>

int main(int argc, char **argv) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;
  
  printf( "\n" );
  printf( "signed char:    %4d - %4d\n", SCHAR_MIN, SCHAR_MAX );
  printf( "unsigned char:  %4u - %4u\n", 0, UCHAR_MAX );
  printf( "\n" );
  printf( "signed short:   %6d - %6d\n", SHRT_MIN, SHRT_MAX );
  printf( "unsigned short: %6u - %6u\n", 0, USHRT_MAX );
  printf( "\n" );
  printf( "signed int:     %11d - %11d\n", INT_MIN, INT_MAX );
  printf( "unsigned int:   %11u - %11u\n", 0, UINT_MAX );
  printf( "\n" );
  printf( "signed long:    %20ld - %20ld\n", LONG_MIN, LONG_MAX );
  printf( "unsigned long:  %20u - %20lu\n", 0, ULONG_MAX );
  printf( "\n" );
  printf( "signed long long:   %20lld - %20lld\n", LLONG_MIN, LLONG_MAX );
  printf( "unsigned long long: %20u - %20llu\n", 0, ULLONG_MAX );
  printf( "\n" );
  printf( "float:          %10f - %10f\n", -FLT_MAX, FLT_MAX );
  printf( "double:         %10f - %10f\n", -DBL_MAX, DBL_MAX );
  printf( "\nlong long: %lu\nfloat: %lu\ndouble: %lu\n", sizeof( long long ), sizeof( float ), sizeof( double ) );

  return 0;
}

