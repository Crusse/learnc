#include <stdio.h>

#define MAXLINE 1000

int _getline( char *str );
void strrev( char *str );

int main(int argc, char **argv) {

  char line[ MAXLINE ] = "";
  
  while ( _getline( line ) ) {
    strrev( line );
    printf( "%s\n", line );
  }

  return 0;
}

int _getline( char *str ) {

  int chr = '\0';
  int i = 0;
  
  while ( ( chr = getchar() ) != EOF && i < MAXLINE - 1 ) {
    
    if ( chr == '\n' || chr == '\r' )
      break;
    
    str[ i ] = chr;
    ++i;
  }
  
  if ( chr == '\n' || chr == '\r' ) {
    str[ i ] = '\0';
    return i;
  }
  
  str[ 0 ] = '\0';

  return 0;
}

void strcopy( char *from, char *to ) {
  
  int i = 0;
  
  for ( i = 0; from[ i ] != '\0'; ++i )
    to[ i ] = from[ i ];
  
  to[ i ] = '\0';
}

int _strlen( char *str ) {

  int len;

  for ( len = 0; str[ len ] != '\0'; ++len ) ;
  
  return len;
}

void strrev( char *str ) {
  
  int len = _strlen( str );
  char strCopy[ len + 1 ];
  
  strcopy( str, strCopy );
  
  for ( int i = 0; i < len; ++i )
    str[ i ] = strCopy[ len - i - 1 ];
}
