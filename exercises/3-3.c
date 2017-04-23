#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define STRMAX 1000

void expand( char *from, char *to ) {
  
  int lastChar = '\0';
  int toI = 0;
  int c = from[ 0 ];
  
  for ( int i = 0; c != '\0'; ++i, c = from[ i ] ) {
    
    if ( c >= 'a' && c <= 'z' ||
      c >= 'A' && c <= 'Z' ||
      c >= '0' && c <= '9' )
    {
      to[ toI++ ] = c;
      lastChar = c;
    }
    else if ( c == '-' && lastChar ) {
      
      int endChar = from[ i + 1 ];
      
      if ( lastChar >= 'a' && lastChar <= 'z' && endChar > lastChar && endChar <= 'z' ||
        lastChar >= 'A' && lastChar <= 'Z' && endChar > lastChar && endChar <= 'Z' ||
        lastChar >= '0' && lastChar <= '9' && endChar > lastChar && endChar <= '9' )
      {
        
        for ( int j = lastChar + 1; j <= endChar; ++j ) {
          to[ toI++ ] = j;
        }
        
        lastChar = endChar;
        i++;
      }
      else {
        to[ toI++ ] = c;
        lastChar = '\0';
      }
    }
    else {
      to[ toI++ ] = c;
      lastChar = '\0';
    }
  }
  
  to[ toI ] = '\0';
}

int main(int argc, char **argv) {
  
  char str1[] = "Something a-z a-b-c here";
  char str2[] = "Foo a-z0-9 bar";
  char str3[] = "-a-z";
  char str4[] = "a-z-";
  char str5[] = "A-z";
  char str6[] = "a-Z";
  char str7[] = "A-Z";

  char expanded[ STRMAX ] = "";
  
  expand( str1, expanded );
  printf( "%s: %s\n", str1, expanded );
  
  expand( str2, expanded );
  printf( "%s: %s\n", str2, expanded );
  
  expand( str3, expanded );
  printf( "%s: %s\n", str3, expanded );
  
  expand( str4, expanded );
  printf( "%s: %s\n", str4, expanded );
  
  expand( str5, expanded );
  printf( "%s: %s\n", str5, expanded );
  
  expand( str6, expanded );
  printf( "%s: %s\n", str6, expanded );
  
  expand( str7, expanded );
  printf( "%s: %s\n", str7, expanded );

  return 0;
}

