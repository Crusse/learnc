#include <stdio.h>
#include <stdlib.h> /* for atof() */
#include <math.h>

#define MAXOP 100 /* max size of operand or operator */
#define NUMBER '0' /* signal that a number was found */

int getop(char []);
void push(double);
double pop(void);

int main(void) {

  int type;
  double op2;
  char s[MAXOP];

  while ((type = getop(s)) != EOF) {
    switch (type) {
    case NUMBER:
      push(atof(s));
      break;
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

#include <ctype.h>

int getch(void);
void ungetch(int);

int getop(char s[]) {

  int chr;

  // Skip whitespace
  while ((s[0] = chr = getch()) == ' ' || chr == '\t') ;

  s[1] = '\0';    

  // Exercise 4-3
  if (!isdigit(chr) && chr != '.' && chr != '-')
    return chr; /* not a number */

  int i = 0;

  if (isdigit(chr) || chr == '-')
    while (isdigit(s[++i] = chr = getch())) ;

  if (chr == '.')
    while (isdigit(s[++i] = chr = getch())) ;

  s[i] = '\0';

  if (chr != EOF)
    ungetch(chr);

  return NUMBER;
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


