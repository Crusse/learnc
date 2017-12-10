#include <stdio.h>
#include <string.h>
#include <ctype.h>

double atof( char str[] ) {

  int i = 0;

  while ( isspace( str[ i ] ) )
    ++i;

  int sign = ( str[ i ] == '-' ) ? -1 : 1;

  if ( str[ i ] == '-' || str[ i ] == '+' )
    ++i;

  double ret = 0;

  for ( ; isdigit( str[ i ] ); ++i )
    ret = ret * 10 + ( str[ i ] - '0' );

  int power = 1;

  if ( str[ i ] == '.' ) {
    ++i;
    for ( ; isdigit( str[ i ] ); ++i ) {
      power *= 10;
      ret = ret * 10 + ( str[ i ] - '0' );
    }
  }

  int magnitude = 0;
  int magnitudeSign = 1;

  if ( str[ i ] == 'e' || str[ i ] == 'E' ) {
    ++i;
    if ( str[ i ] == '-' ) {
      magnitudeSign = -1;
      ++i;
    }
    for ( ; isdigit( str[ i ] ); ++i )
      magnitude = magnitude * 10 + ( str[ i ] - '0' );
  }

  ret = sign * ret / power;

  for ( int j = 0; j < magnitude; ++j ) {
    if ( magnitudeSign < 0 )
      ret /= 10;
    else
      ret *= 10;
  }

  return ret;
}

int main(int argc, char **argv) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;

  printf( "123.0: %f\n", atof( "123.0" ) );
  printf( "0.0: %f\n", atof( "0.0" ) );
  printf( "1.0: %f\n", atof( "1.0" ) );
  printf( "-1.0: %f\n", atof( "-1.0" ) );
  printf( "999.999: %f\n", atof( "999.999" ) );
  printf( "999.999e0: %f\n", atof( "999.999e0" ) );
  printf( "999.999e1: %f\n", atof( "999.999e1" ) );
  printf( "999.999e5: %f\n", atof( "999.999e5" ) );
  printf( "999.999e-3: %f\n", atof( "999.999e-3" ) );
  printf( "999.999e-5: %f\n", atof( "999.999e-5" ) );

  return 0;
}

