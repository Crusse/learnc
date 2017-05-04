#include <stdio.h>
#include <stdlib.h> /* for atof() */
#include <math.h>

#define MAXOP 100 /* max size of operand or operator */
#define OP_UNKNOWN '!'
#define OP_NUMBER '0' /* signal that a number was found */
#define OP_PRINT_TOP 'P'
#define OP_DUP_TOP 'D'
#define OP_SWAP_TOP 'S'

int getop(char []);
void push(double);
double pop(void);
double peek(void);

int main(void) {

  int type;
  double op2;
  char s[MAXOP];

  while ((type = getop(s)) != EOF) {
    
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
      case '\n':
        printf("\t%.8g\n", pop());
        break;
      default:
        printf("error: unknown command %s\n", s);
      break;
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
void ungetch(int);

int str_equal( char str1[], char str2[] ) {
  
  for ( int i = 0; str1[ i ] != '\0' || str2[ i ] != '\0'; ++i ) {
    if ( str1[ i ] != str2[ i ] )
      return 0;
  }
  
  return 1;
}

int getop(char s[]) {

  int chr;
  int i = 0;

  // Skip whitespace
  while ((s[0] = chr = getch()) == ' ' || chr == '\t') ;
  
  s[1] = '\0';
  
  // Exercise 4-3
  if (isdigit(chr) || chr == '.' || chr == '-') {

    if (isdigit(chr) || chr == '-')
      while (isdigit(s[++i] = chr = getch())) ;

    if (chr == '.')
      while (isdigit(s[++i] = chr = getch())) ;

    s[i] = '\0';

    if (chr != EOF)
      ungetch(chr);

    return OP_NUMBER;
  }
  // Exercise 4-4
  else if ( chr >= 'a' && chr <= 'z' ) {
    
    while ( ( chr = getch() ) != EOF && chr >= 'a' && chr <= 'z' ) {
      s[++i] = chr;
    }
    
    s[++i] = '\0';
    
    if ( str_equal( s, "top" ) )
      return OP_PRINT_TOP;
    else if ( str_equal( s, "dup" ) )
      return OP_DUP_TOP;
    else if ( str_equal( s, "swap" ) )
      return OP_SWAP_TOP;
  }
  else {
    return chr;
  }
  
  return OP_UNKNOWN;
}

#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;

int getch(void) {
  return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) {
  if (bufp >= BUFSIZE)
    printf("ungetch: too many characters\n");
  else
    buf[bufp++] = c;
}


