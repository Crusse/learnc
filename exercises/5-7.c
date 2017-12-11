#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAXLINES 5000   // max #lines to be sorted
#define MAXLEN 1000   // max length of any input line

char *lineptr[MAXLINES];  // pointers to text lines

int readlines(char *lineptr[], int nlines);
int readlines_5_7(char *lineptr[], int nlines);
void writelines(char *lineptr[], int nlines);
void qsort(char *lineptr[], int left, int right);

/* sort input lines */
int main(void)
{
  int nlines;    // number of input lines read
  int iterations = 5000000;
  
  char *arr[] = { "aaa", "bbb", "ccc" };
  char arr2[][10] = { "aaa", "bbb", "ccc" };
  
  arr[1][0] = 'd';
  arr2[1][0] = 'd';
  
  printf( "%ld\n", sizeof( arr ) );
  printf( "%ld\n", sizeof( arr2 ) );
  
  printf( "%ld\n", sizeof( int (*)[20] ) );
  printf( "%ld\n", sizeof( int *[10] ) );
  printf( "%ld\n", sizeof( int [10][20] ) );
  
  float startTime = (float) clock() / CLOCKS_PER_SEC;
  for ( int i = 0; i < iterations; ++i ) {
    nlines = readlines(lineptr, MAXLINES);
  }
  printf( "readlines(): %.3f\n", ( (float) clock() / CLOCKS_PER_SEC ) - startTime );
  
  startTime = (float) clock() / CLOCKS_PER_SEC;
  for ( int i = 0; i < iterations; ++i ) {
    nlines = readlines_5_7(lineptr, MAXLINES);
  }
  printf( "readlines_5_7(): %.3f\n", ( (float) clock() / CLOCKS_PER_SEC ) - startTime );
  
  if(nlines >= 0)
  {
    qsort(lineptr, 0, nlines-1);
    writelines(lineptr, nlines);
    return 0;
  }
  else
  {
    printf("error: input too big to sort\n");
    return 1;
  }
}

int _getline(char *, int);
char *alloc(int);

/* readlines: read input lines */
int readlines(char *lineptr[], int maxlines)
{
  int len, nlines;
  char *p, line[MAXLEN];
  
  nlines = 0;
  while ((len = _getline(line, MAXLEN)) > 0) {
    if (nlines >= maxlines || (p = alloc(len)) == NULL)
      return -1;
    else
    {
      line[len-1] = '\0';   // delete newline
      strcpy(p, line);
      lineptr[nlines++] = p;
    }
  }
  
  return nlines;
}

/**
 * There doesn't really seem to be a performance difference between this one
 * and readlines().
 */
int readlines_5_7(char *lineptr[], int maxlines)
{
  int len, nlines;  
  nlines = 0;

  while ((len = _getline(lineptr[nlines], MAXLEN)) > 0) {
    
    if (nlines >= maxlines)
      return -1;

    lineptr[nlines++][len-1] = '\0';   // delete newline
  }
  
  return nlines;
}

/* _getline: read a line into s, return length */
int _getline(char *s, int lim)
{
  int c, i;
  
  for (i = 0; i<lim-1 && (c=getchar()) != EOF && c != '\n'; i++)
    s[i] = c;
  if (c == '\n')
  {
    s[i] = c;
    ++i;
  }
  s[i] = '\0';
  return i;
}

#define ALLOCSIZE 50000           // size of available space

static char allocbuf[ALLOCSIZE];  // storage for alloc 
static char *allocp = allocbuf;   // next free position

char *alloc(int n)
{
  if (allocbuf + ALLOCSIZE - allocp >= n)   // if it fits
  {
    allocp += n;
    return allocp - n;  
  }
  else {
    printf( "No space to alloc\n" );
    return 0;
  }
}

/* writelines: write output lines */
void writelines(char *lineptr[], int nlines)
{
  int i;
  
  for (i = 0; i < nlines; i++)
    printf("%s\n", lineptr[i]);
}

/* qsort: sort v[left]...v[right] into increasing order */
void qsort(char *v[], int left, int right)
{
  int i, last;
  void swap(char *v[], int i, int j);
  
  if (left >= right)  // do nothing if array contains fewer than two elements
    return;
  swap(v, left, (left + right )/2);
  last = left;
  for (i = left+1; i <= right; i++) {
    if (strcmp(v[i], v[left]) < 0)
      swap(v, ++last, i);
  }
  swap(v, left, last);
  qsort(v, left, last-1);
  qsort(v, last+1, right);
}

/* swap: interchange v[i] and v[j] */
void swap(char *v[], int i, int j)
{
  char *temp;
  
  temp = v[i];
  v[i] = v[j];
  v[j] = temp;
}

