#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

int getch(void);
int ungetch(int);
int getfloat( double* );

#define MAX_NUMS 5

int main(int argc, char **argv) {
  
  double nums[ MAX_NUMS ] = { 0 };
  
  for ( int i = 0; i < MAX_NUMS; ++i ) {
    if ( getfloat( &nums[ i ] ) == EOF )
      break;
  }
  
  printf( "-----\n" );

  for ( int i = 0; i < MAX_NUMS; ++i ) {
    printf( "%f\n", nums[ i ] );
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

int getfloat( double *pf ) {
  
  int c, sign;
  
  // Skip whitespace
  while ( isspace( c = getch() ) ) ;
  
  if ( !isdigit( c ) && c != EOF && c != '+' && c != '-' && c != '.' ) {
    ungetch( c );
    return 0;
  }
  
  sign = ( c == '-' ) ? -1 : 1;
  
  if ( c == '+' || c == '-' ) {
    c = getch();

    int nextChr = EOF;

    if ( !isdigit( c ) && ( c != '.' || !isdigit( nextChr = getch() ) ) ) {
      if ( nextChr != EOF )
        ungetch( nextChr );
      if ( c != EOF )
        ungetch( c );
      ungetch( ( sign == -1 ) ? '-' : '+' );
      return ( c == EOF ) ? c : 0;
    }
  }
  
  for ( *pf = 0; isdigit( c ); c = getch() )
    *pf = *pf * 10 + ( c - '0' );
  
  if ( c == '.' )
    c = getch();
  
  int decimalPlaces = 0;
  
  for ( ; isdigit( c ); c = getch(), ++decimalPlaces )
    *pf = *pf * 10 + ( c - '0' );
  
  for ( int i = 0; i < decimalPlaces; ++i )
    *pf /= 10;
  
  *pf *= sign;
  
  if ( c != EOF )
    ungetch( c );
  
  return c;
}

