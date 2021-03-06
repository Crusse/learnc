// Implements 6-2, but doesn't handle typedefs, struct vars or function
// parameters.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_GROUP_KEY_LENGTH 6
#define MAX_GROUP_KEY_LENGTH 31
#define MAX_TOKEN_LEN 254

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

  // Already have this, not adding duplicates
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

void _print_tree( tree_item *tree, int maxKeyLen, char *currKey, char *prevKey ) {

  if ( tree->left )
    _print_tree( tree->left, maxKeyLen, currKey, prevKey );

  int len = strlen( tree->value );
  strncpy( currKey, tree->value, maxKeyLen );
  currKey[ len ] = '\0';

  if ( strcmp( currKey, prevKey ) != 0 )
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

enum {
  T_OTHER,
  T_TYPE_SPECIFIER,
  T_TYPE_QUALIFIER,
  T_STORAGE_CLASS_SPECIFIER,
  T_IDENTIFIER
};

tree_item *tree;
char terminal[ MAX_TOKEN_LEN + 1 ];
int terminalType = T_OTHER;

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

/**
 * Returns the next non-comment, non-string/char-literal character.
 */
int next_char() {

  char c = '\0';

  while ( 1 ) {

    c = getc( stdin );

    if ( c == EOF )
      return c;

    if ( c == '/' ) {

      char lastChar;
      char nextC = getc( stdin );
      ungetc( nextC, stdin );

      // Multi-line comment
      if ( nextC == '*' ) {
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
      // Single-line comment
      else if ( nextC == '/' ) {
        while ( ( c = getc( stdin ) ) != EOF && c != '\n' )
          ;
        continue;
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

    return c;
  }

  return EOF;
}

int consume_char( char toConsume ) {

  char c = next_char();

  if ( c == toConsume )
    return 1;

  ungetc( c, stdin );

  return 0;
}

int is_identifier_start_char( char c ) {
  return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || c == '_';
}

int is_type_qualifier( char *token ) {

  static const char *strs[] = {
    "const",
    "volatile",
  };

  for ( size_t i = 0; i < sizeof strs / ( sizeof *strs ); ++i ) {
    if ( strcmp( strs[ i ], token ) == 0 )
      return 1;
  }

  return 0;
}

int is_type_specifier( char *token ) {

  // Doesn't support structs, unions, enums or typedefs
  static const char *strs[] = {
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

  for ( size_t i = 0; i < sizeof strs / ( sizeof *strs ); ++i ) {
    if ( strcmp( strs[ i ], token ) == 0 )
      return 1;
  }

  return 0;
}

int is_storage_class_specifier( char *token ) {

  // Doesn't support typedef
  static const char *strs[] = {
    "auto",
    "register",
    "static",
    "extern",
  };

  for ( size_t i = 0; i < sizeof strs / ( sizeof *strs ); ++i ) {
    if ( strcmp( strs[ i ], token ) == 0 )
      return 1;
  }

  return 0;
}

int next_alphanum_terminal() {

  char c;
  int i = 0;

  for ( ; i < MAX_TOKEN_LEN; ++i ) {

    c = next_char();

    if ( c == EOF ) {
      terminalType = T_OTHER;
      return 0;
    }

    // A valid var name looks like ^[a-zA-Z_][a-zA-Z0-9_]*$
    if ( !is_identifier_start_char( c ) && ( i == 0 || !( c >= '0' && c <= '9' ) ) ) {
      ungetc( c, stdin );
      break;
    }

    terminal[ i ] = c;
  }

  terminal[ i ] = '\0';

  // Empty string
  if ( !terminal[ 0 ] ) {
    terminalType = T_OTHER;
    return 0;
  }

  if ( is_type_qualifier( terminal ) )
    terminalType = T_TYPE_QUALIFIER;
  else if ( is_type_specifier( terminal ) )
    terminalType = T_TYPE_SPECIFIER;
  else if ( is_storage_class_specifier( terminal ) )
    terminalType = T_STORAGE_CLASS_SPECIFIER;
  else
    terminalType = T_IDENTIFIER;

  return 1;
}

int parse_pointer() {

  int ret = 0;

  consume_space();

  if ( consume_char( '*' ) ) {
    ret = 1;
    consume_space();
    while ( next_alphanum_terminal() && terminalType == T_TYPE_QUALIFIER )
      ;
    parse_pointer();
  }

  return ret;
}

int parse_direct_declarator() {

  consume_space();

  if ( terminalType != T_IDENTIFIER )
    next_alphanum_terminal();

  if ( terminalType == T_IDENTIFIER ) {
    
    consume_space();

    // Ignore function declarations, and function parameters
    if ( consume_char( '(' ) ) {
      int nestingLevel = 0;
      char c;
      while ( ( c = next_char() ) != EOF && ( c != ')' || nestingLevel > 0 ) ) {
        if ( c == '(' )
          ++nestingLevel;
        else if ( c == ')' )
          --nestingLevel;
      }
      return 0;
    }

    tree_add_unique( tree, terminal );

    return 1;
  }
  // Parenthesized variable name
  else if ( consume_char( '(' ) ) {
    
    int parse_declarator();

    return parse_declarator();
  }

  return 0;
}

int parse_declarator() {

  int ret = 0;

  // If parse_pointer() didn't parse an identifier terminal, and the last
  // parsed terminal is an identifier, we know that the terminal is from an
  // earlier identifier and should be invalidated
  if ( !parse_pointer() && terminalType == T_IDENTIFIER )
    terminalType = T_OTHER;

  if ( parse_direct_declarator() )
    ret = 1;

  return ret;
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

      while ( ( c = next_char() ) != EOF && ( ( c != ';' && c != ',' ) || initListNesting > 0 ) ) {
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
  }

  return ret;
}

int parse_declaration_specifiers() {

  int ret = 0;

  while ( next_alphanum_terminal() &&
    ( terminalType == T_STORAGE_CLASS_SPECIFIER ||
      terminalType == T_TYPE_SPECIFIER ||
      terminalType == T_TYPE_QUALIFIER ) )
  {
    ret = 1;
  }

  return ret;
}

int parse_declaration() {

  if ( parse_declaration_specifiers() ) {
    
    consume_space();
    
    if ( parse_init_declarator_list() ) {

      consume_space();

      if ( consume_char( ';' ) )
        return 1;
    }
  }

  return 0;
}

void populate_tree() {

  while ( !feof( stdin ) ) {
    if ( !parse_declaration() )
      next_char();
  }
}

int main( int argc, char *argv[] ) {
  // Prevent GCC warnings
  (void) argc;
  (void) argv;

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

  tree = tree_item_create();
  populate_tree();
  print_tree( tree, keyLength );

  return 0;
}

