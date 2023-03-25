#ifndef LEXER_H_
#define LEXER_H_

#include <stdexcept>
#include <string>

namespace lexer {

class LexerError : public std::runtime_error {
public:
  LexerError(std::string errmsg, size_t line, size_t col)
      : std::runtime_error("Lexer error at [" + std::to_string(line) + ":" +
                           std::to_string(col) + "]: " + errmsg) {}
};

enum TokenType {
  // literals
  TRUE_LITERAL,  // true
  FALSE_LITERAL, // false
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

  WHITESPACE_TOK, // comments, and whitespace.
  END,            // EOF
};

struct Token {
  TokenType type;
  std::string value;
  size_t sline, scol, eline, ecol;
};

class Lexer {
private:
  std::string input;
  size_t line = 0, col = 0;

  // internal, table-driven lexing function
  Token nextToken();

public:
  Lexer(std::string &input) : input(input){};
  Lexer(std::string &&input) : input(std::move(input)){};
  // calls nextToken(), but filters some of the output (whitespace tokens)
  Token GetNextToken();
};

} // end namespace lexer

#endif // LEXER_H_
