#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
  
declaration
 : declaration_specifiers ';'
 | declaration_specifiers init_declarator_list ';'

init_declarator_list
  : init_declarator
  | init_declarator_list ',' init_declarator

init_declarator
  : declarator
  | declarator '=' initializer

declarator
  : pointer direct_declarator
  | direct_declarator

direct_declarator
  : IDENTIFIER
  | '(' declarator ')'
  | direct_declarator '[' constant_expression ']'
  | direct_declarator '[' ']'
  | direct_declarator '(' parameter_type_list ')'
  | direct_declarator '(' identifier_list ')'
  | direct_declarator '(' ')'

declaration_specifiers
  : storage_class_specifier
  | storage_class_specifier declaration_specifiers
  | type_specifier
  | type_specifier declaration_specifiers
  | type_qualifier
  | type_qualifier declaration_specifiers

storage_class_specifier
  : TYPEDEF
  | EXTERN
  | STATIC
  | AUTO
  | REGISTER

type_specifier
  : VOID
  | CHAR
  | SHORT
  | INT
  | LONG
  | FLOAT
  | DOUBLE
  | SIGNED
  | UNSIGNED
  | struct_or_union_specifier
  | enum_specifier
  | TYPE_NAME

type_qualifier
  : CONST
  | VOLATILE

*/

void consume_space() {

  char c;

  while ( ( c = getc( stdin ) ) != EOF ) {
    if ( isspace( c ) )
      continue;
    ungetc( c, stdin );
    break;
  }
}

int get_next_identifier( char *buffer, int bufferLen ) {

  char c = '\0';
  int maxChars = bufferLen - 1;
  int i = 0;

  // A valid var name looks like ^[a-zA-Z_][a-zA-Z0-9_]*$
  for ( ; i < maxChars; ++i ) {

    c = getc( stdin );

    if ( c == EOF ) {
      buffer[ 0 ] = '\0';
      return c;
    }

    if ( !( c >= 'a' && c <= 'z' ) && !( c >= 'A' && c <= 'Z' ) && c != '_' &&
      ( i == 0 || !( c >= '0' && c <= '9' ) ) )
    {
      break;
    }

    buffer[ i ] = c;
  }

  // Allow names for primitive vars and arrays, but not function declarations
  // TODO: temporary hacky solution; do this properly (recursive descent
  // parser -style)
  if ( i == 0 || ( c != ',' && c != ')' && c != ';' ) ) {
    buffer[ 0 ] = '\0';
    return c;
  }

  buffer[ i ] = '\0';

  //printf( "Found name: %s\n", buffer );

  return '\0';
}

typedef struct tree_item {
  char *value;
  struct tree_item *left;
  struct tree_item *right;
} tree_item;

tree_item *tree_item_create() {

  tree_item *item = malloc( sizeof(tree_item) );
  item->value = NULL;
  item->left = NULL;
  item->right = NULL;

  return item;
}

int tree_item_set_value( tree_item *item, const char *str ) {

  int len = strlen( str );
  char *copy = malloc( sizeof(char) * len + 1 );

  if ( copy ) {
    strncpy( copy, str, len );
    copy[ len + 1 ] = '\0';
    item->value = copy;
    return 1;
  }

  return 0;
}

void tree_add_unique( tree_item *tree, const char *str ) {

  if ( tree->value && strcmp( tree->value, str ) == 0 )
    return;

  if ( !tree->value ) {
    tree_item_set_value( tree, str );
  }
  else if ( strcmp( str, tree->value ) < 0 ) {
    if ( !tree->left )
      tree->left = tree_item_create();
    tree_add_unique( tree->left, str );
  }
  else {
    if ( !tree->right )
      tree->right = tree_item_create();
    tree_add_unique( tree->right, str );
  }
}

#define MAX_TOKEN_LEN 254

void populate_tree( tree_item *tree ) {

  int inMultiComment = 0;
  int inLineComment = 0;
  int inString = 0;
  int isEscaping = 0;
  char lastChar = '\0';
  char c = '\0';

  consume_space();

  while ( 1 ) {

    lastChar = c;
    c = getc( stdin );

    if ( c == EOF )
      break;

    if ( inMultiComment ) {
      if ( lastChar == '*' && c == '/' )
        inMultiComment = 0;
      continue;
    }

    if ( inLineComment ) {
      if ( c == '\n' || c == '\r' )
        inLineComment = 0;
      continue;
    }

    if ( inString ) {
      if ( c == '"' && !isEscaping )
        inString = 0;
      if ( isEscaping )
        isEscaping = 0;
      else if ( c == '\\' )
        isEscaping = 1;
      continue;
    }

    if ( lastChar == '/' ) {
      if ( c == '*' )
        inMultiComment = 1;
      else if ( c == '/' )
        inLineComment = 1;
    }
    else if ( c == '"' ) {
      inString = 1;
    }
    else if ( isspace( c ) ) {
      consume_space();
    }
    else {

      char token[ MAX_TOKEN_LEN + 1 ] = { 0 };

      ungetc( c, stdin );
      c = get_next_identifier( token, MAX_TOKEN_LEN + 1 );

      if ( *token )
        tree_add_unique( tree, token );
    }

    lastChar = c;
  }
}

void print_tree( tree_item *tree ) {

  if ( !tree->value )
    return;

  if ( tree->left )
    print_tree( tree->left );

  printf( "%s\n", tree->value );

  if ( tree->right )
    print_tree( tree->right );
}

#define DEFAULT_GROUP_KEY_LENGTH 6

int main( int argc, char *argv[] ) {

  int keyLength = DEFAULT_GROUP_KEY_LENGTH; 

  if ( argc > 1 ) {
    keyLength = atoi( argv[ 1 ] );
    if ( keyLength < 1 ) {
      fprintf( stderr, "Invalid group key length '%s' given", argv[ 1 ] );
      return 1;
    }
  }

  tree_item *tree = tree_item_create();

  populate_tree( tree );
  print_tree( tree );

  return 0;
}

