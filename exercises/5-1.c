#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

int getch(void);
int ungetch(int);
int getint( int* );

#define MAX_NUMS 5

int main(int argc, char **argv) {
  
  int nums[ MAX_NUMS ] = { 0 };
  
  for ( int i = 0; i < MAX_NUMS; ++i ) {
    if ( getint( &nums[ i ] ) == EOF )
      break;
  }
  
  printf( "-----\n" );

  for ( int i = 0; i < MAX_NUMS; ++i ) {
    printf( "%d\n", nums[ i ] );
  }
}

#define CHAR_BUF_SIZE 2
int charBuf[ CHAR_BUF_SIZE ];
int charBufIndex = -1;

int getch(void) {
  if (charBufIndex > -1) {
    return charBuf[ charBufIndex-- ];
  }
  else {
    return getchar();
  }
}

int ungetch(int c) {
  if (charBufIndex + 1 >= CHAR_BUF_SIZE) {
    printf("ungetch: too many characters in buffer\n");
    return 0;
  }
  else {
    if ( c >= 0 )
      charBuf[ ++charBufIndex ] = c;
    return 1;
  }
}

int getint( int *pn ) {
  
  int c, sign;
  
  // Skip whitespace
  while ( isspace( c = getch() ) ) ;
  
  if ( !isdigit( c ) && c != EOF && c != '+' && c != '-' ) {
    ungetch( c );
    return 0;
  }
  
  sign = ( c == '-' ) ? -1 : 1;
  
  if ( c == '+' || c == '-' ) {
    c = getch();
    
    // Exercise 5-1
    if ( !isdigit( c ) ) {
      if ( c != EOF )
        ungetch( c );
      ungetch( ( sign == -1 ) ? '-' : '+' );
      return ( c == EOF ) ? c : 0;
    }
  }
  
  for ( *pn = 0; isdigit( c ); c = getch() ) {
    *pn = *pn * 10 + ( c - '0' );
  }
  
  *pn *= sign;
  
  if ( c != EOF )
    ungetch( c );
  
  return c;
}

