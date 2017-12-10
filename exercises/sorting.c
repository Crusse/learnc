#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_arr( int arr[], int size ) {
  for ( int i = 0; i < size; ++i ) {
    printf( "%d", arr[ i ] );
    if ( i != size - 1 )
      printf( ", " );
  }
  printf( "\n" );
}

void swap( int arr[], int left, int right ) {
  int tmp = arr[ left ];
  arr[ left ] = arr[ right ];
  arr[ right ] = tmp;
}

void bubblesort( int arr[], int size ) {
  
  int swapped;
  
  do {
    
    swapped = 0;
    
    for ( int i = 1; i < size; ++i ) {
      if ( arr[ i - 1 ] > arr[ i ] ) {
        swap( arr, i, i - 1 );
        swapped = 1;
      }
    }
  }
  while ( swapped );
}

void quicksort( int arr[], int size ) {
  
  if ( size < 2 )
    return;
  
  int smallerI = -1;
  int pivot = arr[ size - 1 ];
  
  for ( int i = 0; i < size - 1; ++i ) {
    if ( arr[ i ] <= pivot ) {
      ++smallerI;
      swap( arr, smallerI, i );
    }
  }
  
  swap( arr, smallerI + 1, size - 1 );
  
  quicksort( arr, smallerI + 1 );
  quicksort( arr + smallerI + 2, size - ( smallerI + 2 ) );
}

void quicksort_with_low_high( int arr[], int low, int high ) {
  
  if ( low < high ) {
    
    int smallerI = low - 1;
    int pivot = arr[ high ];
    
    for ( int i = low; i < high; ++i ) {
      if ( arr[ i ] <= pivot ) {
        ++smallerI;
        swap( arr, smallerI, i );
      }
    }
    
    swap( arr, smallerI + 1, high );
    
    quicksort_with_low_high( arr, low, smallerI );
    quicksort_with_low_high( arr, smallerI + 2, high );
  }
}

void init_rand_arr( int arr[], int size ) {
  while ( --size >= 0 ) {
    arr[ size ] = rand() % 10;
  }
}

int main( int argc, char *argv[] ) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;
  
  srand( clock() );
  
  int iters = 1000000;
  double time = 0;
  int arr[10];
  
  for ( int i = 0; i < iters; ++i ) {
    init_rand_arr( arr, 10 );
    if ( i == iters - 1 )
      print_arr( arr, 10 );
    double start = (double) clock() / CLOCKS_PER_SEC;
    bubblesort( arr, 10 );
    time += (double) clock() / CLOCKS_PER_SEC - start;
  }
  print_arr( arr, 10 );
  printf( "Time: %f\n", time );
  time = 0;
  
  for ( int i = 0; i < iters; ++i ) {
    init_rand_arr( arr, 10 );
    if ( i == iters - 1 )
      print_arr( arr, 10 );
    double start = (double) clock() / CLOCKS_PER_SEC;
    quicksort( arr, 10 );
    time += (double) clock() / CLOCKS_PER_SEC - start;
  }
  print_arr( arr, 10 );
  printf( "Time: %f\n", time );
  time = 0;
  
  for ( int i = 0; i < iters; ++i ) {
    init_rand_arr( arr, 10 );
    if ( i == iters - 1 )
      print_arr( arr, 10 );
    double start = (double) clock() / CLOCKS_PER_SEC;
    quicksort_with_low_high( arr, 0, 9 );
    time += (double) clock() / CLOCKS_PER_SEC - start;
  }
  print_arr( arr, 10 );
  printf( "Time: %f\n", time );
  time = 0;
  
  return 0;
}
