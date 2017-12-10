#include <stdio.h>
#include <stdlib.h> /* for atof() */
#include <math.h>

#define MAXOP 100 /* max size of operand or operator */
#define MAXLINE 1000

#define OP_UNKNOWN '!'
#define OP_NUMBER '0' /* signal that a number was found */
#define OP_PRINT_TOP 'P'
#define OP_DUP_TOP 'D'
#define OP_SWAP_TOP 'S'
#define OP_EXP 'e'
#define OP_SIN 's'
#define OP_EXIT 'x'

int getop(char[], char[]);
void push(double);
double pop(void);
double peek(void);
int _getline( char* );

int main( int argc, char *argv[] ) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;

  char line[ MAXLINE ];
  int type;
  double op2;
  char s[MAXOP];

  while ( 1 ) {

    _getline( line );

    while ((type = getop(line, s)) != EOF) {
      
      switch (type) {
        case OP_NUMBER:
          push(atof(s));
          break;
        // Exercise 4-4
        case OP_PRINT_TOP:
          printf( "Top of the stack: %f\n", peek() );
          break;
        case OP_DUP_TOP:
          push( peek() );
          break;
        case OP_SWAP_TOP: {
          double first = pop();
          double second = pop();
          push( first );
          push( second );
          break;
        }
        case '+':
          push(pop() + pop());
          break;
        case '*':
          push(pop() * pop());
          break;
        case '-':
          op2 = pop();
          push(pop() - op2);
          break;
        case '/':
          op2 = pop();
          if (op2 != 0.0)
            push(pop() / op2);
          else
            printf("error: zero divisor\n");
          break;
        // Exercise 4-3
        case '%':
          op2 = pop();
          if (op2 != 0.0)
            push( fmod( pop(), op2 ) );
          else
            printf("error: zero divisor\n");
          break;
        // Exercise 4-5
        case '^':
          op2 = pop();
          push( pow( pop(), op2 ) );
          break;
        case OP_EXP:
          push( exp( pop() ) );
          break;
        case OP_SIN:
          push( sin( pop() ) );
          break;
        case '\n':
          printf("\t%.8g\n", pop());
          break;
        case OP_EXIT:
          return 0;
          break;
        default:
          printf("error: unknown command %s\n", s);
        break;
      }
    }
  }

  return 0;
}

#define MAXVAL 100 /* maximum depth of val stack */

int sp = 0; /* next free stack position */
double val[MAXVAL]; /* value stack */

void push(double f) {
  if (sp < MAXVAL)
    val[sp++] = f;
  else
    printf("error: stack full, can't push %g\n", f);
}

double pop(void) {
  if (sp > 0)
    return val[--sp];
  else {
    printf("error: stack empty\n");
    return 0.0;
  }
}

double peek(void) {
  if (sp > 0)
    return val[sp - 1];
  else {
    return 0.0;
  }
}

#include <ctype.h>

int getch(void);
int ungetch(int);

int str_equal( char str1[], char str2[] ) {
  
  for ( int i = 0; str1[ i ] != '\0' || str2[ i ] != '\0'; ++i ) {
    if ( str1[ i ] != str2[ i ] )
      return 0;
  }
  
  return 1;
}

int _getline( char *str ) {
  
  int chr = '\0';
  int i = 0;
  
  while ( ( chr = getchar() ) != EOF && i < MAXLINE - 1 ) {
    str[ i++ ] = chr;
    if ( chr == '\n' || chr == '\r' )
      break;
  }
  
  if ( chr == '\n' || chr == '\r' ) {
    str[ i ] = '\0';
    return i;
  }
  
  str[ 0 ] = '\0';
  
  return 0;
}

// Exercise 4-10
int getop(char source[], char opOut[]) {
  
  // Exercise 4-11
  static int sourcePointer = 0;
  
  int chr = source[ sourcePointer ];
  
  // Reached end of the given source string
  if ( chr == '\0' || chr == EOF ) {
    opOut[ 0 ] = '\0';
    sourcePointer = 0;
    return EOF;
  }

  // Skip whitespace
  while ( ( chr = source[ sourcePointer ] ) == ' ' || chr == '\t' ) {
    sourcePointer++;
  }

  opOut[0] = chr;
  opOut[1] = '\0';

  int i = 0;
  
  // Exercise 4-3
  if (isdigit(chr) || chr == '.' || chr == '-') {

    if (isdigit(chr) || chr == '-') {
      while (isdigit(chr = source[ ++sourcePointer ]))
        opOut[++i] = chr;
    }

    if (chr == '.') {
      opOut[ ++i ] = chr;
      while (isdigit(chr = source[ ++sourcePointer ]))
        opOut[++i] = chr;
    }

    opOut[++i] = '\0';

    return OP_NUMBER;
  }
  // Exercise 4-4
  else if ( chr >= 'a' && chr <= 'z' ) {
    
    while ( ( chr = source[ ++sourcePointer ] ) >= 'a' && chr <= 'z' ) {
      opOut[++i] = chr;
    }
    
    opOut[++i] = '\0';
    
    if ( str_equal( opOut, "top" ) )
      return OP_PRINT_TOP;
    else if ( str_equal( opOut, "dup" ) )
      return OP_DUP_TOP;
    else if ( str_equal( opOut, "swap" ) )
      return OP_SWAP_TOP;
    // Exercise 4-5
    else if ( str_equal( opOut, "exp" ) )
      return OP_EXP;
    else if ( str_equal( opOut, "sin" ) )
      return OP_SIN;
    else if ( str_equal( opOut, "exit" ) )
      return OP_EXIT;
  }
  else {

    sourcePointer++;

    return chr;
  }
  
  return OP_UNKNOWN;
}

int buf = -1;

int getch(void) {
  // Exercise 4-8
  if (buf > -1) {
    int ret = buf;
    buf = -1;
    return ret;
  }
  else {
    return getchar();
  }
}

int ungetch(int c) {
  if (buf > -1) {
    printf("ungetch: too many characters\n");
    return 0;
  }
  else {
    if ( c >= 0 )
      buf = c;
    return 1;
  }
}

// Exercise 4-7
void ungets( char str[] ) {
  for ( int i = 0; str[ i ] != '\0'; ++i ) {
    if ( !ungetch( str[ i ] ) )
      break;
  }
}


