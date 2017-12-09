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

#define DEFAULT_GROUP_KEY_LENGTH 6
#define MAX_GROUP_KEY_LENGTH 31
#define MAX_TOKEN_LEN 254

#if 0

#define MAX_STACK_SIZE 32

const char *stack[ MAX_STACK_SIZE ] = { 0 };
const char *stackNextFree = stack;

void stack_push( const char *str ) {

  int len = strlen( str );
  char *stackStr = malloc( len + 1 );

  strncpy( stackStr, str, len );
  stackStr[ len ] = '\0';

  ( stackNextFree++ ) = stackStr;
}

void stack_pop() {

  if ( stackNextFree != stack ) {
    free( --stackNextFree );
    return 1;
  }

  return 0;
}

char *stack_current() {

  if ( stackNextFree != stack )
    return stackNextFree - 1;

  return NULL;
}

#endif

char lastToken[ MAX_TOKEN_LEN + 1 ] = { 0 };

int consume_space() {

  char c;
  int ret = 0;

  while ( ( c = getc( stdin ) ) != EOF ) {
    if ( isspace( c ) ) {
      ret = 1;
      continue;
    }
    ungetc( c, stdin );
    break;
  }

  return ret;
}

int consume_char( char toConsume ) {

  char c = getc( stdin );

  if ( c == toConsume )
    return 1;

  ungetc( c, stdin );

  return 0;
}

int is_identifier_start_char( char c ) {
  return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || c == '_';
}

int parse_alphanum_token() {

  char c;
  int i = 0;

  // A valid var name looks like ^[a-zA-Z_][a-zA-Z0-9_]*$
  for ( ; i < MAX_TOKEN_LEN; ++i ) {

    c = next_token_char();

    if ( !c )
      return 0;

    lastToken[ i ] = c;

    if ( !is_identifier_start_char( c ) || ( i != 0 && !( c >= '0' && c <= '9' ) ) ) {
      ungetc( c, stdin );
      return 0;
    }
  }

  lastToken[ i ] = '\0';

  if ( !lastToken[ 0 ] )
    return 0;

  return 1;
}

void parse_direct_declarator() {

}

void parse_pointer() {

  
}

int parse_declarator() {

  int ret = 0;

  consume_space();

  while ( parse_pointer() )
    ;

  consume_space();

  
}

int parse_init_declarator() {

  int ret = 0;

  if ( parse_declarator() ) {

    ret = 1;
    consume_space();

    // Discard initializer
    if ( consume_char( '=' ) ) {

      char c = '\0';
      int initListNesting = 0;

      while ( ( c = getc( stdin ) ) != EOF && c != ';' && ( initListNesting > 0 && c != ',' ) ) {
        if ( c == '{' )
          ++initListNesting;
        else if ( c == '}' )
          --initListNesting;
      }

      if ( c )
        ungetc( c, stdin );
    }
  }

  return ret;
}

int parse_init_declarator_list() {

  int ret = 0;

  while ( parse_init_declarator() ) {

    ret = 1;
    consume_space();

    if ( !consume_char( ',' ) )
      break;

    consume_space();
  }

  return ret;
}

void parse_type_qualifier() {

  static char strs[][] = {
    "const",
    "volatile",
  };

  consume_space();

  if ( parse_alphanum_token() ) {
    for ( int i = 0; i < sizeof strs / ( sizeof *strs ); ++i ) {
      if ( strcmp( strs[ i ], lastToken ) == 0 )
        return 1;
    }
  }

  return 0;
}

void parse_type_specifier() {

  // Doesn't support structs, unions, enums or typedefs
  static char strs[][] = {
    "void",
    "char",
    "short",
    "int",
    "long",
    "float",
    "double",
    "signed",
    "unsigned",
  };

  consume_space();

  if ( parse_alphanum_token() ) {
    for ( int i = 0; i < sizeof strs / ( sizeof *strs ); ++i ) {
      if ( strcmp( strs[ i ], lastToken ) == 0 )
        return 1;
    }
  }

  return 0;
}

int parse_storage_class_specifier() {

  // Doesn't support typedef
  static char strs[][] = {
    "auto",
    "register",
    "static",
    "extern",
  };

  consume_space();

  if ( parse_alphanum_token() ) {
    for ( int i = 0; i < sizeof strs / ( sizeof *strs ); ++i ) {
      if ( strcmp( strs[ i ], lastToken ) == 0 )
        return 1;
    }
  }

  return 0;
}

int parse_declaration_specifiers() {

  int ret = 0;

  while ( parse_storage_class_specifier() || parse_type_specifier() || parse_type_qualifier() )
    ret = 1;

  return ret;
}

int parse_declaration() {

  if ( parse_declaration_specifiers() && parse_init_declarator_list() ) {

    consume_space();

    if ( consume_char( ';' ) ) {
      
    }
  }
}

void parse_token() {

  char c;
  int i = 0;

  // A valid var name looks like ^[a-zA-Z_][a-zA-Z0-9_]*$
  for ( ; i < MAX_TOKEN_LEN; ++i ) {

    c = getc( stdin );

    if ( c == EOF ) {
      token[ 0 ] = '\0';
      return;
    }

    if ( !is_identifier_start_char( c ) || ( i != 0 && !( c >= '0' && c <= '9' ) ) )
      break;

    buffer[ i ] = c;
  }

  // Allow names for primitive vars and arrays, but not function declarations
  if ( i == 0 || ( c != ',' && c != ')' && c != ';' ) ) {
    buffer[ 0 ] = '\0';
    return 0;
  }

  buffer[ i ] = '\0';

  //printf( "Found name: %s\n", buffer );

  return 1;
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
  char *copy = malloc( len + 1 );

  if ( copy ) {
    strncpy( copy, str, len );
    copy[ len ] = '\0';
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

/**
 * Returns the next non-whitespace, non-comment, non-string-literal character.
 */
char next_token_char() {

  char lastChar = '\0';
  char c = '\0';

  consume_space();

  while ( 1 ) {

    c = getc( stdin );

    if ( c == EOF )
      return '\0';

    char nextC;

    if ( c == '/' ) {
      // Multi-line comment
      if ( ( nextC = getc( stdin ) ) == '*' ) {
        lastChar = '\0';
        while ( 1 ) {
          c = getc( stdin );
          if ( c == EOF )
            break;
          if ( lastChar == '*' && c == '/' )
            break;
          lastChar = c;
        }
        continue;
      }
      else {
        ungetc( nextC, stdin );
      }
    }
    else if ( c == '/' ) {
      // Single-line comment
      if ( ( nextC = getc( stdin ) ) == '/' ) {
        while ( ( c = getc( stdin ) ) != EOF && c != '\n' )
          ;
        continue;
      }
      else {
        ungetc( nextC, stdin );
      }
    }
    // String or character literal
    else if ( c == '"' || c == '\'' ) {
      char quoteChar = c;
      int isEscaping = 0;
      while ( ( c = getc( stdin ) ) != EOF && ( c != quoteChar || isEscaping ) ) {
        if ( isEscaping )
          isEscaping = 0;
        else if ( c == '\\' )
          isEscaping = 1;
      }
      continue;
    }
    else if ( isspace( c ) ) {
      consume_space();
      continue;
    }

    return c;
  }

  return '\0';
}

void _print_tree( tree_item *tree, int maxKeyLen, char *currKey, char *prevKey ) {

  if ( tree->left )
    _print_tree( tree->left, maxKeyLen, currKey, prevKey );

  int len = strlen( tree->value );
  strncpy( currKey, tree->value, maxKeyLen );
  currKey[ len ] = '\0';

  if ( strcmp( key, prevKey ) != 0 )
    printf( "-----------------------\n" );

  printf( "%s\n", tree->value );
  strncpy( prevKey, currKey, maxKeyLen );
  prevKey[ len ] = '\0';

  if ( tree->right )
    _print_tree( tree->right, maxKeyLen, currKey, prevKey );
}

void print_tree( tree_item *tree, int maxKeyLen ) {

  if ( !tree->value )
    return;

  char currKey[ MAX_GROUP_KEY_LENGTH + 1 ] = { 0 };
  char prevKey[ MAX_GROUP_KEY_LENGTH + 1 ] = { 0 };

  _print_tree( tree, maxKeyLen, currKey, prevKey );
}

int main( int argc, char *argv[] ) {

  int keyLength = DEFAULT_GROUP_KEY_LENGTH; 

  if ( argc > 1 ) {
    keyLength = atoi( argv[ 1 ] );
    if ( keyLength < 1 ) {
      fprintf( stderr, "Invalid group key length '%s' given\n", argv[ 1 ] );
      return 1;
    }
    if ( keyLength > MAX_GROUP_KEY_LENGTH ) {
      fprintf( stderr, "Group key length must be <= %d\n", MAX_GROUP_KEY_LENGTH );
      return 1;
    }
  }

  tree_item *tree = tree_item_create();

  populate_tree( tree );
  print_tree( tree, keyLength );

  return 0;
}

