#include <stdio.h>

#define MAXSTRSIZE 100

char *_strncpy( char *s, char *t, int n ) {

  char *ret = s;

  for ( int i = 0; i < n; ++i ) {
    *( s++ ) = ( *t == '\0' )
      ? '\0'
      : *( t++ );
  }

  *s = '\0';

  return ret;
}

char *_strncat( char *s, char *t, int n ) {

  char *ret = s;

  while ( *s != '\0' )
    s++;

  for ( int i = 0; i < n && *t != '\0'; ++i ) {
    *( s++ ) = *( t++ );
  }

  *s = '\0';

  return ret;
}

int _strncmp( char *s, char *t, int n ) {

  for ( int i = 0; i < n; ++i ) {
    if ( *s < *t )
      return -1;
    if ( *s > *t )
      return 1;
    if ( *s == '\0' )
      return 0;
    s++;
    t++;
  }

  return 0;
}

int main( int argc, char **argv ) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;

  char s1[] = "foo";
  char s2[ MAXSTRSIZE ] = "bar";
  char s3[ MAXSTRSIZE ];
  char *str;

  str = _strncpy( s3, s1, 2 );
  str = _strncat( s3, s2, 2 );

  printf( "%s\n", str );
  printf( "%d\n", _strncmp( str, "foc", 3 ) );
  printf( "%d\n", _strncmp( str, "fobb", 3 ) );
  printf( "%d\n", _strncmp( str, "foba", 3 ) );
  printf( "%d\n", _strncmp( str, "foaa", 3 ) );
  printf( "%d\n", _strncmp( str, "", 3 ) );

  return 0;
}

