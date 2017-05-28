#include <stdio.h>
#include <string.h>

#define MAXLINES 5000
char *lineptr[MAXLINES];

int readlines(char *lineptr[], int nlines);
void writelines(char *lineptr[], int nlines);

void qsort2(void *lineptr[], int left, int right, int (*comp)(void *, void *));
int numcmp(char *, char *);
void reverse( void *v[], int size );
void swap(void *v[], int, int);

int main(int argc, char *argv[])
{
  int nlines;
  int numeric = 0;
  int rev = 0;
  
  // Exercise 5-14
  for ( int i = 1; i < argc; ++i ) {
    char *argStr = argv[ i ];
    if ( argStr[ 0 ] == '-' )
      argStr++;
    while ( argStr[ 0 ] != '\0' ) {
      switch ( argStr[ 0 ] ) {
        case 'n':
          numeric = 1;
          break;
        case 'r':
          rev = 1;
          break;
      }
      argStr++;
    }
  }

  if((nlines = readlines(lineptr, MAXLINES)) >= 0)
  {
    qsort2(
      (void **) lineptr,
      0,
      nlines-1,
      numeric ? (int (*)(void*, void*)) numcmp : (int (*)(void*, void*)) strcmp
    );
    // Exercise 5-14
    if ( rev )
      reverse( (void **) lineptr, nlines );
    writelines(lineptr, nlines);
    return 0;
  }
  else
  {
    printf("input too big to sort\n");
    return 1;
  }
}

void reverse( void *v[], int size ) {
  int last = size - 1;
  int half = last / 2;
  for ( int i = 0; i <= half; ++i ) {
    swap( v, i, last - i );
  }
}

void qsort2(void *v[], int left, int right, int(*comp)(void *, void *))
{
  int i, last;
  
  if(left >= right)
    return;
  
  swap(v, left, (left + right) / 2);
  last = left;
  
  for(i = left + 1; i <= right; i++) {
    if((*comp)(v[i], v[left]) < 0)
      swap(v, ++last, i);
  }
  
  swap(v, left, last);
  
  qsort2(v, left, last - 1, comp);
  qsort2(v, last + 1, right, comp);
}

#include <stdlib.h>

int numcmp(char *s1, char *s2)
{
  double v1, v2;
  
  v1 = atof(s1);
  v2 = atof(s2);
  if (v1 < v2)
    return -1;
  else if (v1 > v2)
    return 1;
  else
    return 0;
}

void swap(void *v[], int i, int j)
{
  void *temp;
  
  temp = v[i];
  v[i] = v[j];
  v[j] = temp;
}


#define MAXLEN 1000   // max length of any input line
int _getline(char *, int);
char *alloc(int);

/* readlines: read input lines */
int readlines(char *lineptr[], int maxlines)
{
  int len, nlines;
  char *p, line[MAXLEN];
  
  nlines = 0;
  while ((len = _getline(line, MAXLEN)) > 0)
    if (nlines >= maxlines || (p = alloc(len)) == NULL)
      return -1;
    else
    {
      line[len-1] = '\0';   // delete newline
      strcpy(p, line);
      lineptr[nlines++] = p;
    }
    return nlines;
}

/* getline: read a line into s, return length */
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
  else
    return 0;
}

/* writelines: write output lines */
void writelines(char *lineptr[], int nlines)
{
  int i;
  
  for (i = 0; i < nlines; i++)
    printf("%s\n", lineptr[i]);
}
