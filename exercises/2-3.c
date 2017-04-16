#include <stdio.h>

int htoi( char hex[] ) {
  
  int i = 0;
  
  if ( hex[ 0 ] == '0' && ( hex[ 1 ] == 'x' || hex[ 1 ] == 'X' ) )
    i = 2;
  
  int chr = hex[ i ];
  int ret = 0;
  
  for ( ; chr != '\0'; ++i, chr = hex[ i ] ) {
    
    int decimal;
    
    if ( chr >= '0' && chr <= '9' )
      decimal = chr - '0';
    else if ( chr >= 'a' && chr <= 'f' )
      decimal = chr - 'a' + 10;
    else if ( chr >= 'A' && chr <= 'F' )
      decimal = chr - 'A' + 10;
    else
      return -1;
    
    ret = ret * 16 + decimal;
  }
  
  return ret;
}

int main(int argc, char **argv) {
  
  printf( "%d\n", htoi( "0xFF" ) );
  printf( "%d\n", htoi( "0XFF" ) );
  printf( "%d\n", htoi( "0xff" ) );
  printf( "%d\n", htoi( "0Xff" ) );
  printf( "%d\n", htoi( "FF" ) );
  printf( "%d\n", htoi( "ff" ) );
  printf( "%d\n", htoi( "ffff" ) );
  printf( "%d\n", htoi( "0" ) );
  printf( "%d\n", htoi( "0xx" ) );
  printf( "%d\n", htoi( "x" ) );
  printf( "%d\n", htoi( "xx" ) );
  
  return 0;
}

