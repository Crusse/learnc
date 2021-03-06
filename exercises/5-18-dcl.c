#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "5-18.h"

int tokentype;   /* type of last token */
char token[MAXTOKEN];    /* last token string */
char name[MAXTOKEN]; /* identifier name */
char datatype[MAXTOKEN]; /* char, int, etc. */
char out[1000]; /* output string */

/* dcl: parse a declarator  */
void dcl(void)
{
  int ns;
  for(ns = 0; gettoken() == '*'; )     /* count *'s    */
    ns++;
  if ( tokentype == '\n' )
    return;
  dirdcl();
  while(ns-- > 0)
    strcat(out," pointer to");
}

/* dirdcl: parse a direct declaration */
void dirdcl(void)
{
  int type;
  
  if(tokentype == '(' )
  {
    dcl();
    if(tokentype != ')')
      printf("error: missing )\n");
  }
  else if (tokentype == NAME)
    strcpy(name,token);
  else
    printf("error: expected name or (dcl)\n");
  
  while((type = gettoken()) == PARENS || type == BRACKETS) {
    if(type == PARENS)
      strcat(out, " function returning");
    else
    {
      strcat(out, " array");
      strcat(out, token);
      strcat(out, " of");
    }
  }
}

int gettoken(void)
{
  int c;
  char *p = token;
  
  while((c=getch()) == ' ' || c == '\t')
    ;
  
  if(c == '(')
  {
    if((c = getch()) == ')')
    {
      strcpy(token,"()");
      return tokentype = PARENS;
    }
    else
    {
      ungetch(c);
      return tokentype = '(';
    }
  }
  else if (c == '[')
  {
    for(*p++ = c; (*p++ = getch()) != ']'; )
      ;
    *p ='\0';
    return tokentype = BRACKETS;
  }
  else if (isalpha(c))
  {
    for(*p++ = c; isalnum(c=getch()); )
      *p++ = c;
    *p = '\0';
    ungetch(c);
    return tokentype = NAME;
  }
  else
    return tokentype = c;
}

void resetdcl() {
  tokentype = 0;
  token[0] = '\0';
  name[0] = '\0';
  datatype[0] = '\0';
  out[0] = '\0';
}

