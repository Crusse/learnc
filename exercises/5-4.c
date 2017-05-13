#include <stdio.h>

int strend( char *s, char *t ) {

  int sLen = 0;
  int tLen = 0;

  while ( *s != '\0' ) {
    s++;
    sLen++;
  }

  while ( *t != '\0' ) {
    t++;
    tLen++;
  }

  if ( !tLen || tLen > sLen )
    return 0;

  while ( --tLen >= 0 ) {
    if ( *( --s ) != *( --t ) )
      return 0;
  }

  return 1;
}

int main( int argc, char **argv ) {

  printf( "%d\n", strend( "foobar", "baz" ) );
  printf( "%d\n", strend( "foobar", "bar" ) );
  printf( "%d\n", strend( "foobar", "foobar" ) );
  printf( "%d\n", strend( "foobar", "ffoobar" ) );
  printf( "%d\n", strend( "foobar", "foobarr" ) );
  printf( "%d\n", strend( "foobar", "" ) );
  printf( "%d\n", strend( "", "bar" ) );

  return 0;
}

