#include <stdio.h>
#include <limits.h>
#include <string.h>

void _reverse( char str[], int left, int last ) {
  
  int right = last - left;
  char tmp = str[ right ];

  str[ right ] = str[ left ];
  str[ left ] = tmp;
  
  if ( left + 1 < right )
    _reverse( str, left + 1, last );
}

void reverse( char str[] ) {
  _reverse( str, 0, strlen( str ) - 1 );
}

int main(int argc, char **argv) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;
  
  char str[] = "abcd1234";
  reverse( str );
  printf( "%s\n", str );
  
  char str2[] = "12345";
  reverse( str2 );
  printf( "%s\n", str2 );
  
  char str3[] = "12";
  reverse( str3 );
  printf( "%s\n", str3 );
  
  char str4[] = "1";
  reverse( str4 );
  printf( "%s\n", str4 );
  
  char str5[] = "123";
  reverse( str5 );
  printf( "%s\n", str5 );
}

