#include <stdio.h>
#include <string.h>

int strrindex( char haystack[], char needle[] ) {

  int ret = -1;

  for ( int i = 0; haystack[ i ] != '\0' && haystack[ i ] != '\n'; ++i ) {

    int j = 0;

    for ( ; needle[ j ] != '\0' && needle[ j ] == haystack[ i + j ]; ++j ) ;

    if ( j && needle[ j ] == '\0' )
      ret = i;
  }

  return ret;
}

int main(int argc, char **argv) {

  printf( "%d\n", strrindex( "abcdefg", "abc" ) );
  printf( "%d\n", strrindex( "abcabcd", "abc" ) );
  printf( "%d\n", strrindex( "abcabcd", "efg" ) );

  return 0;
}

