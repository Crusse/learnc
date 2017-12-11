#include <stdio.h>

void squeeze( char str[], char removed[] ) {
  
  int len = 0;
  
  for ( int i = 0; str[ i ] != '\0'; ++i ) {
    
    int remove = 0;
    
    for ( int j = 0; removed[ j ] != '\0'; ++j ) {
      if ( removed[ j ] == str[ i ] ) {
        remove = 1;
        break;
      }
    }
    
    if ( !remove )
      str[ len++ ] = str[ i ];
  }
  
  str[ len ] = '\0';
}

int main(int argc, char **argv) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;
  
  char str[] = "a1b23cd4";
  
  printf( "\"%s\" -> ", str );
  
  squeeze( str, "1234567890" );
  
  printf( "\"%s\"\n", str );
  
  return 0;
}

