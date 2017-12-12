// Same as 6-2.c, but supports function parameters. This implementation also
// has a parser that I consciously tried to make a recursive descent, top-down,
// backtracking parser (whereas 6-2.c was totally ad-hoc), although I'm still
// not totally sure if that's how you'd describe this parser.
//
// If PRINT_FUNCS = 1, there are some false-positives (function _calls_ are
// printed as function declarations/definitions). 6-2 only asks for variables
// names, though, so therefore PRINT_FUNCS is 0.
//
// "Recursive-descent can handle any grammar which is LL(*) (that is, unlimited
// lookahead) as well as a small set of ambiguous grammars. This is because
// recursive-descent is actually a directly-encoded implementation of PEGs, or
// Parser Expression Grammar(s)."
//   https://stackoverflow.com/questions/1044600/difference-between-an-ll-and-recursive-descent-parser

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_GROUP_KEY_LENGTH 6
#define MAX_GROUP_KEY_LENGTH 31
#define MAX_TOKEN_LEN 127
#define MAX_BACKTRACK_TOKENS 512
#define PRINT_VARS 1
#define PRINT_FUNCS 0
#define PRINT_PARAMS 1

typedef enum token_type_t {
  T_EOF,
  T_OTHER,
  T_TYPE_SPECIFIER,
  T_TYPE_QUALIFIER,
  T_STORAGE_CLASS_SPECIFIER,
  T_PAREN_L,
  T_PAREN_R,
  T_BRACKET_L,
  T_BRACKET_R,
  T_BRACE_L,
  T_BRACE_R,
  T_SEMICOLON,
  T_COMMA,
  T_ASTERISK,
  T_SINGLE_EQUALS,
  T_IDENTIFIER,
} token_type_t;

typedef struct token_t {
  token_type_t type;
  char text[ MAX_TOKEN_LEN + 1 ];
} token_t;

// We keep a buffer of the lexed tokens, so that we can backtrack and
// not have to re-lex the tokens again
token_t tokenBuffer[ MAX_BACKTRACK_TOKENS ];
token_t *token = &tokenBuffer[ MAX_BACKTRACK_TOKENS - 1 ];
token_t *backtrackPoint = &tokenBuffer[ MAX_BACKTRACK_TOKENS - 1 ];

void save_backtrack_point() {
  backtrackPoint = token;
}

void backtrack() {

  if ( !backtrackPoint || backtrackPoint == tokenBuffer ) {
    fprintf( stderr, "Cannot backtrack beyond token \"%s\"\n", ( backtrackPoint ? backtrackPoint->text : "" ) );
    exit( 1 );
  }

  token = backtrackPoint;

  //printf( "Backtracked to \"%s\"\n", token->text );
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

void discard_bracket_until_char( char endChar ) {

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
  
  int nestingLevel = 0;
  char tokenChar;
  void next_token();
  next_token();

  while ( token->type != T_EOF ) {
    tokenChar = token->text[ 0 ];
    if ( tokenChar == endChar && nestingLevel == 0 ) {
      next_token();
      break;
    }
    if ( tokenChar == startChar )
      ++nestingLevel;
    else if ( tokenChar == endChar )
      --nestingLevel;
    next_token();
  }
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

/**
 * This function is a bit of a hack. If we had proper typedef processing we
 * wouldn't need this, but we currently do, as we detect type specifiers (int
 * etc.) as generic identifiers.
 */
int is_invalid_identifier( char *token ) {

  static const char *strs[] = {
    // Too lazy to add all of the keywords right now
    "typedef",
    "return",
    "if",
    "else",
  };

  for ( size_t i = 0; i < sizeof strs / ( sizeof *strs ); ++i ) {
    if ( strcmp( strs[ i ], token ) == 0 )
      return 1;
  }

  return 0;
}

void next_token() {

  // There is a token already in the buffer (backtrack() was called earlier)
  if ( token != tokenBuffer + ( MAX_BACKTRACK_TOKENS - 1 ) ) {
    ++token;
    //printf( "Token (buffered): %2d, %s\n", token->type, token->text );
    return;
  }

  // Shift all elements in the buffer to the left (i.e. discard the oldest
  // token and make space for a new one)
  memmove( tokenBuffer, tokenBuffer + 1, sizeof tokenBuffer[ 0 ] * ( MAX_BACKTRACK_TOKENS - 1 ) );

  if ( backtrackPoint == tokenBuffer ) {
    fprintf( stderr, "Reached token buffer limit (oldest token: \"%s\")\n", backtrackPoint->text );
    exit( 1 );
  }

  --backtrackPoint;

  token->type = T_EOF;

  while ( 1 ) {

    consume_space();

    int c = getchar();

    if ( c == EOF )
      break;

    if ( c == '/' ) {

      int nextC = getchar();

      // Multi-line comment
      if ( nextC == '*' ) {
        int lastChar = '\0';
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
      // A literal forward slash
      else {
        ungetc( nextC, stdin );
        strcpy( token->text, "/" );
        token->type = T_OTHER;
        break;
      }
    }
    // String or character literal
    else if ( c == '"' || c == '\'' ) {

      int quoteChar = c;
      c = getchar();
      while ( c != EOF && c != quoteChar ) {
        if ( c == '\\' )
          getchar();
        c = getchar();
      }

      strcpy( token->text, "<string>" );
      token->type = T_OTHER;
      break;
    }
    // [a-zA-Z_]
    else if ( is_identifier_start_char( c ) ) {

      int i = 0;

      for ( ; ; ++i ) {

        // A valid var name looks like ^[a-zA-Z_][a-zA-Z0-9_]*$
        if ( !is_identifier_start_char( c ) && ( i == 0 || !( c >= '0' && c <= '9' ) ) ) {
          ungetc( c, stdin );
          break;
        }

        if ( i >= MAX_TOKEN_LEN ) {
          token->text[ MAX_TOKEN_LEN ] = '\0';
          fprintf( stderr, "Reached MAX_TOKEN_LEN for the token \"%s\"\n", token->text );
          exit( 1 );
        }

        token->text[ i ] = c;

        c = getchar();
        if ( c == EOF )
          break;
      }

      token->text[ i ] = '\0';

      if ( is_type_qualifier( token->text ) )
        token->type = T_TYPE_QUALIFIER;
      else if ( is_storage_class_specifier( token->text ) )
        token->type = T_STORAGE_CLASS_SPECIFIER;
      else if ( is_invalid_identifier( token->text ) )
        token->type = T_OTHER;
      else
        token->type = T_IDENTIFIER;

      break;
    }
    else {

      token->text[ 0 ] = c;
      token->text[ 1 ] = '\0';

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
      else if ( c == '{' )
        token->type = T_BRACE_L;
      else if ( c == '}' )
        token->type = T_BRACE_R;
      else if ( c == '=' ) {
        char nextC = getchar();
        if ( nextC == '=' ) {
          strcpy( token->text, "==" );
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

  //printf( "Token: %13d, %s\n", token->type, token->text );
}

token_t *accept_token( token_type_t type ) {

  if ( token->type == type ) {
    //printf( "Accepted %d, \"%s\"\n", token->type, token->text );
    next_token();
    return token - 1;
  }

  //printf( "Rejected %d, \"%s\" (looking for %d)\n", token->type, token->text, type );

  return NULL;
}

token_t *accept_token_by_char( char c ) {

  if ( token->text[ 0 ] == c ) {
    next_token();
    return token - 1;
  }

  return NULL;
}

int peek_token( token_type_t type ) {
  return token->type == type;
}

struct tree_item_t;
struct tree_item_t tree;
void tree_add_unique( struct tree_item_t *, const char * );

typedef enum decltype_t {
  D_VAR,
  D_PARAM,
  D_FUNC_DEF,
  D_FUNC_DECL,
} decltype_t;

typedef struct declaration_t {
  decltype_t type;
  char identifier[ MAX_TOKEN_LEN + 1 ];
} declaration_t;

void add_declaration_to_identifier_tree( decltype_t declType, char *identifier ) {

#define MAX_TYPE_INFO_CHARS 30

  char str[ MAX_TOKEN_LEN + 1 + MAX_TYPE_INFO_CHARS ] = { 0 };
  strncpy( str, identifier, MAX_TOKEN_LEN );

  if ( 0 ) {}

#if PRINT_VARS
  else if ( declType == D_VAR )
    tree_add_unique( &tree, strncat( str, " (var)", MAX_TYPE_INFO_CHARS ) );
#endif

#if PRINT_PARAMS
  else if ( declType == D_PARAM )
    tree_add_unique( &tree, strncat( str, " (parameter)", MAX_TYPE_INFO_CHARS ) );
#endif

#if PRINT_FUNCS
  else if ( declType == D_FUNC_DEF )
    tree_add_unique( &tree, strncat( str, " (function definition)", MAX_TYPE_INFO_CHARS ) );
  else if ( declType == D_FUNC_DECL )
    tree_add_unique( &tree, strncat( str, " (function declaration)", MAX_TYPE_INFO_CHARS ) );
#endif
}

///////////////////////////////////////////////////////////
// Parsing of all C grammar productions that we care about.
// This is where we care about the semantics of the lexed
// tokens. The most important function is parse_direct_declarator(),
// where we collect identifier names.
///////////////////////////////////////////////////////////

// "An abstract declarator is a declarator without an identifier."
// We want identifiers, so we'll just discard these.
//
// direct_abstract_declarator
// 	: '(' abstract_declarator ')'
// 	| '[' ']'
// 	| '[' constant_expression ']'
// 	| direct_abstract_declarator '[' ']'
// 	| direct_abstract_declarator '[' constant_expression ']'
// 	| '(' ')'
// 	| '(' parameter_type_list ')'
// 	| direct_abstract_declarator '(' ')'
// 	| direct_abstract_declarator '(' parameter_type_list ')'
//
int parse_direct_abstract_declarator() {

  if ( accept_token( T_PAREN_L ) ) {
    discard_bracket_until_char( ')' );
    return 1;
  }
  else if ( accept_token( T_BRACKET_L ) ) {
    discard_bracket_until_char( ']' );
    return 1;
  }

  return 0;
}

// parameter_declaration
//  : declaration_specifiers declarator
//  | declaration_specifiers abstract_declarator
//  | declaration_specifiers
int parse_parameter_declaration() {

  int parse_declaration_specifiers();

  if ( parse_declaration_specifiers() ) {

    declaration_t declaration;
    int parse_declarator( declaration_t* );

    if ( parse_declarator( &declaration ) )
      add_declaration_to_identifier_tree( D_PARAM, declaration.identifier );
    else
      parse_direct_abstract_declarator();

    return 1;
  }

  return 0;
}

// parameter_list
//  : parameter_declaration
//  | parameter_list ',' parameter_declaration
//
// Note: this has a left-recursive rule, which I changed into two right-recursive
// productions like this:
//
// parameter_list
//  : parameter_declaration parameter_list_rest_opt
//
// parameter_list_rest
//  : ',' parameter_list
//  | empty
//
int parse_parameter_list() {

  if ( parse_parameter_declaration() ) {

    if ( accept_token( T_COMMA ) )
      parse_parameter_list();

    return 1;
  }

  return 0;
}

// parameter_type_list
//  : parameter_list
//  | parameter_list ',' ELLIPSIS
int parse_parameter_type_list() {

  if ( parse_parameter_list() ) {

    if ( accept_token( T_COMMA ) ) {
      // Match ... (three dots)
      while ( accept_token_by_char( '.' ) )
        ;
    }

    return 1;
  }

  return 0;
}

// From what I can tell, this production only exists in C to support old
// versions of the C language:
// https://stackoverflow.com/questions/18202232/c-function-with-parameter-without-type-indicator-still-works
//
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

  token_t *token = accept_token( T_IDENTIFIER );

  if ( token ) {

    add_declaration_to_identifier_tree( D_PARAM, token->text );

    if ( accept_token( T_COMMA ) )
      parse_identifier_list();

    return 1;
  }

  return 0;
}

int parse_direct_declarator_parens() {

  if ( accept_token( T_PAREN_L ) ) {

    if ( accept_token( T_PAREN_R ) )
      return 1;

    if ( parse_parameter_type_list() || parse_identifier_list() )
      return 1;
  }

  return 0;
}

int parse_direct_declarator_brackets() {

  if ( accept_token( T_BRACKET_L ) ) {
    // Discard everything inside brackets
    discard_bracket_until_char( ']' );
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
int parse_direct_declarator( declaration_t *declaration ) {

  // Parenthesized identifier
  if ( accept_token( T_PAREN_L ) ) {
    int parse_declarator( declaration_t* );
    return parse_declarator( declaration ) && accept_token( T_PAREN_R );
  }

  token_t *token = accept_token( T_IDENTIFIER );

  if ( token ) {

    strncpy( declaration->identifier, token->text, MAX_TOKEN_LEN );
    declaration->identifier[ MAX_TOKEN_LEN ] = '\0';
    declaration->type = D_VAR;

    if ( !parse_direct_declarator_brackets() ) {
      if ( parse_direct_declarator_parens() )
        declaration->type = D_FUNC_DECL;
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
int parse_declarator( declaration_t *declaration ) {

  parse_pointer();

  if ( parse_direct_declarator( declaration ) )
    return 1;

  return 0;
}

// init_declarator
//  : declarator
//  | declarator '=' initializer
//
// Note: the left-most symbol for both rules is the same. We always try the
// longest rule, like (f)lex does:
// https://stackoverflow.com/a/8409541
// https://en.wikipedia.org/wiki/Maximal_munch
//
int parse_init_declarator() {

  declaration_t declaration;

  if ( parse_declarator( &declaration ) ) {

    // Discard initializer
    if ( accept_token( T_SINGLE_EQUALS ) ) {

      int initListNesting = 0;

      while ( token->type != T_EOF ) {
        if ( ( token->type == T_COMMA || token->type == T_SEMICOLON ) && initListNesting == 0 ) {
          next_token();
          break;
        }
        if ( token->type == T_BRACE_L )
          ++initListNesting;
        else if ( token->type == T_BRACE_R )
          --initListNesting;
        next_token();
      }
    }

    if ( peek_token( T_COMMA ) || peek_token( T_SEMICOLON ) )
      add_declaration_to_identifier_tree( declaration.type, declaration.identifier );

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

    if ( accept_token( T_COMMA ) )
      parse_init_declarator_list();

    return 1;
  }

  return 0;
}

// declaration_specifiers
//  : storage_class_specifier declaration_specifiers_opt
//  | type_specifier declaration_specifiers_opt
//  | type_qualifier declaration_specifiers_opt
int parse_declaration_specifiers() {

  int hasTypeSpecifier = ( token != tokenBuffer && ( token - 1 )->type == T_IDENTIFIER );

  if ( accept_token( T_STORAGE_CLASS_SPECIFIER ) ||
    // To support typedef'd types, we accept any valid identifier as a type
    // specifier (rather than only looking for built-in types like "int" etc.)
    //
    // To support typedef's type names, we lexed the type as a T_IDENTIFIER,
    // in next_token(). If there are no pointer after such a token, but another
    // T_IDENTIFIER, we'll assume that the first T_IDENTIFIER is actually
    // a T_TYPE_SPECIFIER.
    //
    ( !hasTypeSpecifier && accept_token( T_IDENTIFIER ) ) ||
    accept_token( T_TYPE_QUALIFIER ) )
  {
    parse_declaration_specifiers();
    return 1;
  }

  return 0;
}

/**
 * Variable declarations.
 */
int parse_declaration() {

  if ( parse_declaration_specifiers() ) {
    if ( parse_init_declarator_list() ) {
      if ( accept_token( T_SEMICOLON ) )
        return 1;
    }
  }

  return 0;
}

/**
 * C89-style function parameter type declarations.
 */
int parse_declaration_list() {

  if ( parse_declaration() ) {
    parse_declaration_list();
    return 1;
  }

  return 0;
}

int parse_compound_statement() {

  if ( accept_token( T_BRACE_L ) )
    return 1;

  return 0;
}

int parse_function_definition() {

  // C89 allows functions without return type (they default to int), so the
  // declaration specifiers are optional
  parse_declaration_specifiers();

  declaration_t declaration;

  if ( parse_declarator( &declaration ) ) {
    // C89-style function parameter type declarations
    parse_declaration_list();
    if ( parse_compound_statement() && declaration.type == D_FUNC_DECL ) {
      declaration.type = D_FUNC_DEF;
      add_declaration_to_identifier_tree( declaration.type, declaration.identifier );
      return 1;
    }
  }

  return 0;
}

void parse_translation_unit() {

  next_token();

  while ( 1 ) {

    if ( token->type == T_EOF )
      break;

    save_backtrack_point();

    if ( parse_function_definition() ) {

      //printf( "Found function definition\n" );
    }
    else {

      //printf( "Not a function definition\n" );
      backtrack();

      if ( parse_declaration() ) {
        //printf( "Found declaration\n" );
      }
      else {
        //printf( "Not a declaration\n" );
        backtrack();
        next_token();
        save_backtrack_point();
      }
    }
  }
}

///////////////////////////////////////////////////////////
// Binary tree for identifier names
///////////////////////////////////////////////////////////

typedef struct tree_item_t {
  char *value;
  struct tree_item_t *left;
  struct tree_item_t *right;
} tree_item_t;

tree_item_t tree;

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

///////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////

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

  parse_translation_unit();
  print_tree( &tree, keyLength );

  return 0;
}

