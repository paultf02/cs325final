#pragma once
#include <string>
#include <cstdio>
#include <deque>

//===----------------------------------------------------------------------===//
// Lexer
//===----------------------------------------------------------------------===//

extern FILE *pFile;

extern std::string IdentifierStr; // Filled in if IDENT
extern int IntVal;                // Filled in if INT_LIT
extern bool BoolVal;              // Filled in if BOOL_LIT
extern float FloatVal;            // Filled in if FLOAT_LIT
extern std::string StringVal;     // Filled in if String Literal
extern int lineNo, columnNo; // not static because used in mccomp.cpp



// The lexer returns one of these for known things.
enum TOKEN_TYPE {

  IDENT = -1,        // [a-zA-Z_][a-zA-Z_0-9]*
  ASSIGN = int('='), // '='

  // delimiters
  LBRA = int('{'),  // left brace
  RBRA = int('}'),  // right brace
  LPAR = int('('),  // left parenthesis
  RPAR = int(')'),  // right parenthesis
  SC = int(';'),    // semicolon
  COMMA = int(','), // comma

  // types
  INT_TOK = -2,   // "int"
  VOID_TOK = -3,  // "void"
  FLOAT_TOK = -4, // "float"
  BOOL_TOK = -5,  // "bool"

  // keywords
  EXTERN = -6,  // "extern"
  IF = -7,      // "if"
  ELSE = -8,    // "else"
  WHILE = -9,   // "while"
  RETURN = -10, // "return"

  // dont use these because we have BOOL_LIT
  // // // // // // // TRUE   = -12,     // "true"
  // // // // // // // FALSE   = -13,     // "false"

  // literals
  INT_LIT = -14,   // [0-9]+
  FLOAT_LIT = -15, // [0-9]+.[0-9]+
  BOOL_LIT = -16,  // "true" or "false" key words

  // logical operators
  AND = -17, // "&&"
  OR = -18,  // "||"

  // operators
  PLUS = int('+'),    // addition or unary plus
  MINUS = int('-'),   // substraction or unary negative
  ASTERIX = int('*'), // multiplication
  DIV = int('/'),     // division
  MOD = int('%'),     // modular
  NOT = int('!'),     // unary negation

  // comparison operators
  EQ = -19,      // equal
  NE = -20,      // not equal
  LE = -21,      // less than or equal to
  LT = int('<'), // less than
  GE = -23,      // greater than or equal to
  GT = int('>'), // greater than

  // special tokens
  EOF_TOK = 0, // signal end of file

  // invalid
  INVALID = -100 // signal invalid token
};

// TOKEN struct is used to keep track of information about a token
struct TOKEN {
  int type = -100;
  std::string lexeme;
  int lineNo;
  int columnNo;
};

extern TOKEN CurTok;
extern std::deque<TOKEN> tok_buffer;

TOKEN returnTok(std::string lexVal, int tok_type);
TOKEN gettok();

bool noerr_str_to_bool(std::string str);

TOKEN getNextToken();
void putBackToken(TOKEN tok);
