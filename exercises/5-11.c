#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define STACK_MAX 100

/**
 * ./5-11 2 3 4 + '*'  evalutes  2*(3+4)
 */
int main( int argc, char *argv[] ) {

  double operands[ STACK_MAX ] = { 0 };
  int opCount = 0;

  // Skip the name of the program
  argc--;
  argv++;

  while ( argc > 0 ) {

    if ( strlen( *argv ) == 1 && !isdigit( **argv ) ) {

      if ( opCount < 2 ) {
        printf( "Not enough operands\n" );
        return 1;
      }
      
      double op2 = operands[ --opCount ];
      double op1 = operands[ --opCount ];

      switch ( **argv ) {
        case '+':
          operands[ opCount++ ] = op1 + op2;
          break;
        case '-':
          operands[ opCount++ ] = op1 - op2;
          break;
        case '*':
          operands[ opCount++ ] = op1 * op2;
          break;
        case '/':
          operands[ opCount++ ] = op1 / op2;
          break;
        default:
          printf( "Invalid operator '%c'\n", **argv );
          return 1;
      }
    }
    else {

      if ( opCount >= STACK_MAX ) {
        printf( "Operand stack size exceeded\n" );
        return 1;
      }

      operands[ opCount++ ] = atof( *argv );
    }

    argc--;
    argv++;
  }

  printf( "%lf\n", operands[ opCount - 1 ] );

  return 0;
}

