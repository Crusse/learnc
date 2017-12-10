#include <stdio.h>

void _strcat( char *s, char *t ) {

  while ( *s != '\0' )
    s++;

  while ( ( *(s++) = *(t++) ) != '\0' ) ;
}

int main( int argc, char **argv ) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;

  char s1[] = "foo";
  char s2[] = "bar";

  _strcat( s1, s2 );
  printf( "%s\n", s1 );

  return 0;
}

