#include <stdio.h>
#include <limits.h>
#include <string.h>

#define swap(TYPE, A, B) {\
  TYPE tmp = A;\
  A = B;\
  B = tmp;\
}

int main(int argc, char **argv) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;
  
  int i = 1;
  int j = 2;
  
  printf( "%d, %d\n", i, j );
  swap( int, i, j );
  printf( "%d, %d\n", i, j );
}

