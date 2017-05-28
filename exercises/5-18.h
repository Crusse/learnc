#ifndef __5_18__
#define __5_18__

//
// 5-18-dcl.c
//

#define MAXTOKEN 100

enum { NAME, PARENS, BRACKETS };

int gettoken(void);
extern int tokentype;   /* type of last token */
extern char token[];    /* last token string */
extern char name[]; /* identifier name */
extern char datatype[]; /* char, int, etc. */
extern char out[]; /* output string */

void dcl(void);
void dirdcl(void);

//
// 5-18-getch.c
//
int getch(void);
void ungetch(int);

#endif
