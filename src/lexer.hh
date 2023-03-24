#ifndef LEXER_H_
#define LEXER_H_

#include <stdexcept>

namespace lexer {

class LexerError : public std::runtime_error {
  // inherit constructors.
  using std::runtime_error::runtime_error;
};

enum CharClass {
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  L,
  N,
  O,
  P,
  R,
  S,
  T,
  U,
  W,
  X,
  Y,
  HASH,        // #
  UNDERSCORE,  // _
  COMMA,       // ,
  STAR,        // *
  DIV,         // /
  PLUS,        // +
  MINUS,       // -
  GREATER,     // >
  LESS,        // <
  EQ,          // =
  EXCLAMATION, // !
  COLON,       // :
  SEMICOLON,   // ;
  DOT,         // .
  LBRACKET,    // (
  RBRACKET,    // )
  LBRACE,      // {
  RBRACE,      // }
  ALPHA,       // [a-z] that doesn't have its own character class.
  DIGIT,       // [0-9]
};

enum Token {
  // literals
  TRUE,  // true
  FALSE, // false
  INTEGER_LITERAL,
  FLOAT_LITERAL,
  COLOUR_LITERAL,

  IDENTIFIER,

  // typenames
  FLOAT,  // float
  INT,    // int
  BOOL,   // bool
  COLOUR, // colour

  // keywords
  PAD_WIDTH,  // __width
  PAD_HEIGHT, // __height
  READ,       // __read
  RANDI,      // __randi
  LET,        // let
  PRINT,      // __print
  DELAY,      // __delay
  PIXELR,     // __pixelr
  PIXEL,      // __pixel
  RETURN,     // return
  IF,         // if
  ELSE,       // else
  FOR,        // for
  WHILE,      // while
  FUN,        // fun

  // operators, special characters
  COMMA_TOK,     // ,
  GREATER_TOK,   // >
  LESS_TOK,      // <
  EQ_TOK,        // ==
  NEQ_TOK,       // !=
  GE,            // >=
  LE,            // <=
  PLUS_TOK,      // +
  MINUS_TOK,     // -
  STAR_TOK,      // *
  DIV_TOK,       // /
  ASSIGN,        // =
  AND,           // and
  OR,            // or
  NOT,           // not
  LBRACKET_TOK,  // (
  RBRACKET_TOK,  // )
  LBRACE_TOK,    // {
  RBRACE_TOK,    // }
  ARROW,         // ->
  COLON_TOK,     // :
  SEMICOLON_TOK, // ;
  DOT_TOK,       // .

};

CharClass characterClass(char c);

class Lexer {
public:
  Token GetNextToken();
};

} // end namespace lexer

#endif // LEXER_H_
