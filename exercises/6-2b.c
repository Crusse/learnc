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
#define MAX_TOKEN_LEN 1023
#define TOKEN_BUFFER_MAX 2
#define TOKEN_BUFFER_USE_MALLOC 0
#define DECL_STACK_MAX 32
#define PRINT_VARS 1
#define PRINT_PARAMS 1
#define PRINT_FUNCS 0

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
  T_PAREN_L,
  T_PAREN_R,
  T_BRACKET_L,
  T_BRACKET_R,
  T_SEMICOLON,
  T_COMMA,
  T_ASTERISK,
  T_SINGLE_EQUALS,
  T_IDENTIFIER,
};

tree_item_t *tree;

typedef struct token_t {
  int type;
  char value[ MAX_TOKEN_LEN + 1 ];
} token_t;

#if TOKEN_BUFFER_USE_MALLOC

// Somewhat like a ring buffer
token_t *tokenBuffer[ TOKEN_BUFFER_MAX ];
// One of the elements of tokenBuffer
token_t *token = tokenBuffer[ TOKEN_BUFFER_MAX - 1 ];

#else

token_t tokenBuffer[ TOKEN_BUFFER_MAX ];
token_t *token = &tokenBuffer[ TOKEN_BUFFER_MAX - 1 ];

#endif

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

void next_token() {

  // There is a token already in the buffer (backtrack_token() was called earlier)
#if TOKEN_BUFFER_USE_MALLOC
  if ( token != tokenBuffer[ TOKEN_BUFFER_MAX - 1 ] ) {
#else
  if ( token != tokenBuffer + ( TOKEN_BUFFER_MAX - 1 ) ) {
#endif
    ++token;
    return;
  }

#if TOKEN_BUFFER_USE_MALLOC
  free( tokenBuffer[ 0 ] );
  token = malloc( sizeof (token_t) );
#endif

  // Shift all elements in the buffer to the left (i.e. discard the oldest
  // token)
  memmove( tokenBuffer, tokenBuffer + 1, sizeof tokenBuffer[ 0 ] * ( TOKEN_BUFFER_MAX - 1 ) );

#if TOKEN_BUFFER_USE_MALLOC
  tokenBuffer[ TOKEN_BUFFER_MAX - 1 ] = token;
#endif

  token->type = EOF;

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
        strcpy( token->value, "/" );
        token->type = T_OTHER;
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

      token->value[ 0 ] = '\0';
      token->type = T_OTHER;
      break;
    }
    // [a-zA-Z_]
    else if ( is_identifier_start_char( c ) ) {

      int i = 0;

      for ( ; ; ++i ) {

        c = getchar();

        if ( c == EOF )
          break;

        // A valid var name looks like ^[a-zA-Z_][a-zA-Z0-9_]*$
        if ( !is_identifier_start_char( c ) && ( i == 0 || !( c >= '0' && c <= '9' ) ) ) {
          ungetc( c, stdin );
          break;
        }

        if ( i >= MAX_TOKEN_LEN ) {
          token->value[ MAX_TOKEN_LEN ] = '\0';
          fprintf( stderr, "Reached MAX_TOKEN_LEN for the token \"%s\"\n", token->value );
        }

        token->value[ i ] = c;
      }

      token->value[ i ] = '\0';

      // Empty string
      if ( !token->value[ 0 ] )
        token->type = T_OTHER;
      else if ( is_type_qualifier( token->value ) )
        token->type = T_TYPE_QUALIFIER;
      else if ( is_type_specifier( token->value ) )
        token->type = T_TYPE_SPECIFIER;
      else if ( is_storage_class_specifier( token->value ) )
        token->type = T_STORAGE_CLASS_SPECIFIER;
      else
        token->type = T_IDENTIFIER;

      break;
    }
    else {

      token->value[ 0 ] = c;
      token->value[ 1 ] = '\0';

      if ( c == ';' )
        token->type = T_SEMICOLON;
      else if ( c == ',' )
        token->type = T_COMMA;
      else if ( c == '*' )
        token->type = T_ASTERISK;
      else if ( c == '(' )
        token->type = T_PAREN_L;
      else if ( c == ')' )
        token->type = T_PAREN_R;
      else if ( c == '[' )
        token->type = T_BRACKET_L;
      else if ( c == ']' )
        token->type = T_BRACKET_R;
      else if ( c == '=' ) {
        char nextC = getchar();
        if ( nextC == '=' ) {
          strcpy( token->value, "==" );
          token->type = T_OTHER;
        }
        else {
          ungetc( nextC, stdin );
          token->type = T_SINGLE_EQUALS;
        }
      }
      else
        token->type = T_OTHER;

      break;
    }
  }
}

FIXME
int consume_char( char toConsume ) {

  char c = getchar();

  if ( c == toConsume )
    return 1;

  ungetc( c, stdin );

  return 0;
}

int accept_token( token_type_t type ) {

  if ( token->type == type ) {
    next_token();
    return 1;
  }

  return 0;
}

int peek_token( token_type_t type ) {
  return token->type == type;
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

typedef enum decltype_t {
  D_IDENTIFIER,
  D_POINTER,
  D_FUNC,
  D_PARAM,
} decltype_t;

typedef struct decl_t {
  decltype_t type;
  char *identifier[ MAX_TOKEN_LEN + 1 ];
} decl_t;

decl_t *declStack[ DECL_STACK_MAX ];

int parse_identifier_list_rest() {

  if ( accept_token( T_COMMA ) ) {
    parse_identifier_list();
    return 1;
  }

  return 0;
}

// identifier_list
//  : IDENTIFIER
//  | identifier_list ',' IDENTIFIER
//
// Note: this has a left-recursive rule, which I changed into two right-recursive
// productions like this:
//
// identifier_list
//  : IDENTIFIER identifier_list_rest_opt
//
// identifier_list_rest
//  : ',' identifier_list
//  | empty
//
int parse_identifier_list() {

  if ( accept_token( T_IDENTIFIER ) ) {
    parse_identifier_list_rest();
    return 1;
  }

  return 0;
}

int parse_parameter_type_list() {


}

int parse_direct_declarator_brackets() {

  if ( accept_token( T_BRACKET_L ) ) {
    // Discard everything inside brackets
    consume_until_bracket_end( ']' );
    next_token();
    return 1;
  }

  return 0;
}

int parse_direct_declarator_parens() {

  if ( accept_token( T_PAREN_L ) ) {
    if ( accept_token( T_PAREN_R ) || parse_parameter_type_list() || parse_identifier_list() )
      return 1;
  }

  return 0;
}

// direct_declarator
//  : IDENTIFIER
//  | '(' declarator ')'
//  | direct_declarator '[' constant_expression ']'
//  | direct_declarator '[' ']'
//  | direct_declarator '(' parameter_type_list ')'
//  | direct_declarator '(' identifier_list ')'
//  | direct_declarator '(' ')'
int parse_direct_declarator() {

  int haveIdentifier = 0;

  // Parenthesized identifier
  if ( accept_token( T_PAREN_L ) ) {
    int parse_declarator();
    haveIdentifier = parse_declarator();
  }
  // Identifier
  else {
    //FIXME tree_add_unique( tree, token->value );
    haveIdentifier = accept_token( T_IDENTIFIER );
  }

  if ( haveIdentifier ) {

    if ( parse_direct_declarator_parens() ) {
      return 1;
    }
    else if ( parse_direct_declarator_brackets() ) {
      return 1;
    }
    else {

    }

    return 1;
  }

  return 0;
}

// pointer
//  : '*'
//  | '*' type_qualifier_list
//  | '*' pointer
//  | '*' type_qualifier_list pointer
int parse_pointer() {

  if ( accept_token( T_ASTERISK ) ) {
    while ( accept_token( T_TYPE_QUALIFIER ) )
      ;
    parse_pointer();
    return 1;
  }

  return 0;
}

// declarator
//  : pointer direct_declarator
//  | direct_declarator
int parse_declarator() {

  parse_pointer();

  if ( parse_direct_declarator() )
    return 1;

  return 0;
}

// init_declarator
//  : declarator
//  | declarator '=' initializer
//
// Note: the left-most symbol for both rules is the same. We always try the
// longest rule first, like (f)lex does: https://stackoverflow.com/a/8409541
int parse_init_declarator() {

  if ( parse_declarator() ) {

    // Discard initializer
    if ( accept_token( T_SINGLE_EQUALS ) ) {

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

    return 1;
  }

  return 0;
}

int parse_init_declarator_list_rest() {

  if ( accept_token( T_COMMA ) ) {
    parse_init_declarator_list();
    return 1;
  }

  return 0;
}

// init_declarator_list
//  : init_declarator
//  | init_declarator_list ',' init_declarator
//
// Note: this production is left-recursive. Left-recursion causes infinite
// recursion for top-down parsers (e.g. recursive descent parsers):
// http://www.kavinder.com/blog/2015-06-01-understanding-left-v-right-recursion/
//
// Therefore the code here implements the following right-recursive production,
// where I've changed the left-recursion to right-recursion:
//
// init_declarator_list
//  : init_declarator init_declarator_list_rest
//
// init_declarator_list_rest
//  : ',' init_declarator_list
//  | empty
//
int parse_init_declarator_list() {

  if ( parse_init_declarator() ) {
    parse_init_declarator_list_rest();
    return 1;
  }

  return 0;
}

// declaration_specifiers
//  : storage_class_specifier declaration_specifiers_opt
//  | type_specifier declaration_specifiers_opt
//  | type_qualifier declaration_specifiers_opt
int parse_declaration_specifiers() {

  if ( accept_token( T_STORAGE_CLASS_SPECIFIER ) ||
    accept_token( T_TYPE_SPECIFIER ) ||
    accept_token( T_TYPE_QUALIFIER ) )
  {
    parse_declaration_specifiers();
    return 1;
  }

  return 0;
}

int parse_declaration() {

  if ( parse_declaration_specifiers() ) {
    if ( parse_init_declarator_list() ) {
      if ( accept_token( T_SEMICOLON ) )
        return 1;
    }
  }

  return 0;
}

void parse_translation_unit() {

  while ( 1 ) {
    next_token();
    if ( token->type == T_EOF )
      break;
    parse_declaration();
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

