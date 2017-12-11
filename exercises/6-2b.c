// Same as 6-2.c, but supports function parameters. This implementation also
// has a parser that I consciously tried to make a recursive descent, top-down,
// LL(1) parser (whereas 6-2.c was totally ad-hoc) although I'm still not totally
// sure if that's how you'd describe this parser.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_GROUP_KEY_LENGTH 6
#define MAX_GROUP_KEY_LENGTH 31
#define MAX_TOKEN_LEN 1022
#define TOKEN_BUFFER_MAX 2

typedef struct tree_item_t {
  char *value;
  struct tree_item_t *left;
  struct tree_item_t *right;
} tree_item_t;

tree_item_t *tree_item_create() {

  tree_item_t *item = malloc( sizeof (tree_item_t) );
  item->value = NULL;
  item->left = NULL;
  item->right = NULL;

  return item;
}

int tree_item_set_value( tree_item_t *item, const char *str ) {

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

void tree_add_unique( tree_item_t *tree, const char *str ) {

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

void _print_tree( tree_item_t *tree, int maxKeyLen, char *currKey, char *prevKey ) {

  if ( tree->left )
    _print_tree( tree->left, maxKeyLen, currKey, prevKey );

  strncpy( currKey, tree->value, maxKeyLen );
  currKey[ maxKeyLen ] = '\0';

  if ( strcmp( currKey, prevKey ) != 0 )
    printf( "-----------------------\n" );

  printf( "%s\n", tree->value );
  strncpy( prevKey, currKey, maxKeyLen );
  prevKey[ maxKeyLen ] = '\0';

  if ( tree->right )
    _print_tree( tree->right, maxKeyLen, currKey, prevKey );
}

void print_tree( tree_item_t *tree, int maxKeyLen ) {

  if ( !tree->value )
    return;

  char currKey[ MAX_GROUP_KEY_LENGTH + 1 ] = { 0 };
  char prevKey[ MAX_GROUP_KEY_LENGTH + 1 ] = { 0 };

  _print_tree( tree, maxKeyLen, currKey, prevKey );
}

enum token_type_t {
  T_EOF,
  T_OTHER,
  T_TYPE_SPECIFIER,
  T_TYPE_QUALIFIER,
  T_STORAGE_CLASS_SPECIFIER,
  T_PARENS,
  T_BRACKETS,
  T_IDENTIFIER
};

tree_item_t *tree;

typedef struct token_t {
  int type;
  char value[ MAX_TOKEN_LEN + 1 ];
} token_t;

token_t *create_token( char *value, enum token_type_t type ) {
  
  token_t *ret = malloc( sizeof (token_t) );

  strncpy( ret->value, value, MAX_TOKEN_LEN );
  ret->value[ MAX_TOKEN_LEN ] = '\0';
  ret->type = type;

  return ret;
}

void destroy_token( token_t *token ) {
  free( token );
}

// Somewhat like a ring buffer
token *tokenBuffer[ TOKEN_BUFFER_MAX ];
// One of the elements of tokenBuffer
token *token;

void backtrack_token() {

  if ( !token || token == tokenBuffer[ 0 ] ) {
    fprintf( stderr, "Nothing to backtrack_token() to\n" );
    exit( 1 );
  }

  --token;
}

int consume_space() {

  char c;
  int ret = 0;

  while ( ( c = getchar() ) != EOF ) {
    if ( isspace( c ) ) {
      ret = 1;
      continue;
    }
    ungetc( c, stdin );
    break;
  }

  return ret;
}

void consume_until_bracket_end( char endChar ) {

  int nestingLevel = 0;
  char c;
  char startChar;

  if ( endChar == ')' )
    startChar = '(';
  else if ( endChar == ']' )
    startChar = '[';
  else if ( endChar == '}' )
    startChar = '{';
  else {
    fprintf( stderr, "Invalid endChar '%c'\n", endChar );
    exit( 1 );
  }

  while ( ( c = getchar() ) != EOF && ( c != endChar || nestingLevel > 0 ) ) {
    if ( c == startChar )
      ++nestingLevel;
    else if ( c == endChar )
      --nestingLevel;
  }
}

void peek_token() {

}

void next_token() {

  if ( token && token != tokenBuffer[ TOKEN_BUFFER_MAX - 1 ] ) {
    ++token;
    return;
  }

  char tokenValue[ MAX_TOKEN_LEN + 1 ] = { 0 };
  int tokenType = T_EOF;

  while ( 1 ) {

    consume_space();

    char c = getchar();

    if ( c == EOF )
      break;

    if ( c == '/' ) {

      char nextC = getchar();
      ungetc( nextC, stdin );

      // Multi-line comment
      if ( nextC == '*' ) {
        char lastChar = '\0';
        while ( ( c = getchar() ) != EOF ) {
          if ( lastChar == '*' && c == '/' )
            break;
          lastChar = c;
        }
      }
      // Single-line comment
      else if ( nextC == '/' ) {
        while ( ( c = getchar() ) != EOF && c != '\n' )
          ;
      }
      else {
        // A literal forward slash
        strcpy( tokenValue, "/" );
        tokenType = T_OTHER;
        break;
      }
    }
    // String or character literal
    else if ( c == '"' || c == '\'' ) {

      char quoteChar = c;
      int isEscaping = 0;
      while ( ( c = getchar() ) != EOF && ( c != quoteChar || isEscaping ) ) {
        if ( isEscaping )
          isEscaping = 0;
        else if ( c == '\\' )
          isEscaping = 1;
      }
    }
    else if ( c === '(' ) {

      consume_until_bracket_end( ')' );
      strcpy( tokenValue, ")" );
      tokenType = T_PARENS;

      break;
    }
    else if ( c === '[' ) {

      consume_until_bracket_end( ']' );
      strcpy( tokenValue, "]" );
      tokenType = T_BRACKETS;

      break;
    }
    // [a-zA-Z_]
    else if ( is_identifier_start_char( c ) ) {

      int i = 0;

      for ( ; i < MAX_TOKEN_LEN; ++i ) {

        c = getchar();

        if ( c == EOF )
          break;

        // A valid var name looks like ^[a-zA-Z_][a-zA-Z0-9_]*$
        if ( !is_identifier_start_char( c ) && ( i == 0 || !( c >= '0' && c <= '9' ) ) ) {
          ungetc( c, stdin );
          break;
        }

        tokenValue[ i ] = c;
      }

      tokenValue[ i ] = '\0';

      // Empty string
      if ( !tokenValue[ 0 ] )
        tokenType = T_OTHER;
      else if ( is_type_qualifier( tokenValue ) )
        tokenType = T_TYPE_QUALIFIER;
      else if ( is_type_specifier( tokenValue ) )
        tokenType = T_TYPE_SPECIFIER;
      else if ( is_storage_class_specifier( tokenValue ) )
        tokenType = T_STORAGE_CLASS_SPECIFIER;
      else
        tokenType = T_IDENTIFIER;

      break;
    }
    else {

      tokenValue[ 0 ] = c;
      tokenValue[ 1 ] = '\0';
      tokenType = T_OTHER;

      break;
    }
  }

  // Shift all elements in the buffer to the left (i.e. discard the oldest
  // token)
  for ( int i = 0; i < TOKEN_BUFFER_MAX - 1; ++i ) {
    if ( tokenBuffer[ i ] )
      destroy_token( tokenBuffer[ i ] );
    tokenBuffer[ i ] = tokenBuffer[ i + 1 ];
  }

  token = tokenBuffer[ TOKEN_BUFFER_MAX - 1 ] = create_token( tokenValue, tokenType );
}

int consume_char( char toConsume ) {

  char c = getchar();

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

int parse_pointer() {

  int ret = 0;

  consume_space();

  while ( consume_char( '*' ) ) {
    ret = 1;
    consume_space();
    while ( next_alphanum_token() && tokenType == T_TYPE_QUALIFIER && consume_space() )
      parse_pointer();
  }

  return ret;
}

int parse_direct_declarator() {

  consume_space();

  // We might've already found an identifier when looking for a type qualifier
  // in parse_pointer()
  if ( tokenType != T_IDENTIFIER )
    next_alphanum_token();

  if ( tokenType == T_IDENTIFIER ) {
    
    consume_space();

    // Ignore function declarations, and function parameters
    if ( consume_char( '(' ) ) {
      int nestingLevel = 0;
      char c;
      while ( ( c = getchar() ) != EOF && ( c != ')' || nestingLevel > 0 ) ) {
        if ( c == '(' )
          ++nestingLevel;
        else if ( c == ')' )
          --nestingLevel;
      }
      return 0;
    }

    tree_add_unique( tree, token );

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

  while ( parse_pointer() )
    ;

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

      while ( ( c = getchar() ) != EOF && ( ( c != ';' && c != ',' ) || initListNesting > 0 ) ) {
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

  while ( next_alphanum_token() &&
    ( tokenType == T_STORAGE_CLASS_SPECIFIER ||
      tokenType == T_TYPE_SPECIFIER ||
      tokenType == T_TYPE_QUALIFIER ) )
  {
    ret = 1;
  }

  return ret;
}

int parse_declaration() {

  next_token();

  ...TODO...

  parse_declaration_specifiers(); && consume_space() && parse_init_declarator_list() ) {

    consume_space();

    if ( consume_char( ';' ) )
      return 1;
  }

  return 0;
}

void populate_tree() {

  while ( !feof( stdin ) ) {
    if ( !parse_declaration() )
      getchar();
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

