#ifndef LEXER_H_
#define LEXER_H_

#include "location.hh"
#include "util.hh"

#include <iostream>
#include <stdexcept>
#include <string>

namespace lexer
{

  class LexerError : public CompilationError
  {
  public:
    LexerError(std::string errmsg, size_t line, size_t col)
        : CompilationError("Lexer error at [" + std::to_string(line) + ":" +
                           std::to_string(col) + "]: " + errmsg) {}
  };

  enum TokenType
  {
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
    NEWARR,     // __newarr
    LET,        // let
    PRINT,      // __print
    DELAY,      // __delay
    PIXELR,     // __pixelr
    PIXEL,      // __pixel
    GETCHAR,    // __getchar
    PUTCHAR,    // __putchar
    FLOAT2INT,  // __float2int
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
    LSQBRACE_TOK,  // [
    RSQBRACE_TOK,  // ]
    ARROW,         // ->
    COLON_TOK,     // :
    SEMICOLON_TOK, // ;

    WHITESPACE_TOK, // comments, and whitespace.
    END,            // EOF
  };

  std::string to_string(TokenType tokType);

  struct Token
  {
    TokenType type;
    std::string value;
    Location loc;
  };

  class Lexer
  {
  private:
    std::istream &input;
    size_t line = 1, col = 0;

    // internal, table-driven lexing function
    Token nextToken();

  public:
    Lexer(std::istream &input) : input(input) {};
    // calls nextToken(), but filters some of the output (whitespace tokens)
    Token getNextToken();
  };

} // end namespace lexer

#endif // LEXER_H_
