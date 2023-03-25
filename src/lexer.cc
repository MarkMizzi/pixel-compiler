#include "lexer.hh"

#include <cctype>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

namespace lexer {

enum LexerState {
  START,

  ERROR_STATE,

  // literals
  INTEGER_LITERAL_STATE,
  FLOAT_LITERAL_STATE,
  COLOUR_LITERAL_STATE,

  IDENTIFIER_STATE,

  // operators
  COMMA_STATE,
  GREATER_STATE,
  LESS_STATE,
  EQ_STATE,
  NEQ_STATE,
  GE_STATE,
  LE_STATE,
  PLUS_STATE,
  MINUS_STATE,
  STAR_STATE,
  DIV_STATE,
  ASSIGN_STATE,
  LBRACKET_STATE,
  RBRACKET_STATE,
  LBRACE_STATE,
  RBRACE_STATE,
  ARROW_STATE,
  COLON_STATE,
  SEMICOLON_STATE,
  DOT_STATE,

};

enum CharClass {
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
  ALPHA,       // [a-zA-Z]
  DIGIT,       // [0-9]
};

using LexerTransitionTable =
    std::map<std::pair<LexerState, CharClass>, LexerState>;

// return the token type corresponding to the final state of the lexer.
TokenType tokenType(LexerState finalState);

// return the character class of c.
CharClass characterClass(char c);

// the lexer's transition table
static const LexerTransitionTable tt{{{START, COMMA}, COMMA_STATE},
                                     {{START, GREATER}, GREATER_STATE},
                                     {{GREATER_STATE, EQ}, GE_STATE},
                                     {{START, LESS}, LESS_STATE},
                                     {{LESS_STATE, EQ}, LE_STATE}};

static const std::map<std::string, TokenType> keywords{
    {"true", TokenType::TRUE_LITERAL},
    {"false", TokenType::FALSE_LITERAL},
    {"float", TokenType::FLOAT},
    {"int", TokenType::INT},
    {"bool", TokenType::BOOL},
    {"colour", TokenType::COLOUR},
    {"__width", TokenType::PAD_WIDTH},
    {"__height", TokenType::PAD_HEIGHT},
    {"__read", TokenType::READ},
    {"__randi", TokenType::RANDI},
    {"let", TokenType::LET},
    {"__print", TokenType::PRINT},
    {"__delay", TokenType::DELAY},
    {"__pixelr", TokenType::PIXELR},
    {"__pixel", TokenType::PIXEL},
    {"return", TokenType::RETURN},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"for", TokenType::FOR},
    {"while", TokenType::WHILE},
    {"fun", TokenType::FUN},
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"not", TokenType::NOT}};

Token Lexer::GetNextToken() {
  Token token;
  // filter out whitespace tokens.
  while ((token = nextToken()).type == WHITESPACE)
    ;
  // check if an identifier is actually a keyword.
  if (token.type == TokenType::IDENTIFIER && keywords.count(token.value)) {
    token.type = keywords.at(token.value);
  }
  return token;
}

Token Lexer::nextToken() {
  LexerState state = START;
  Token token;

  if (input.empty()) {
    return {TokenType::END, ""};
  }

  while (!input.empty()) {
    char c = input[0];
    CharClass cclass = characterClass(c);

    if (!tt.count({state, cclass})) {
      // no transition to make
      break;
    }

    state = tt.at({state, cclass});
    token.value.push_back(c);
    input.erase(0, 1);
  }

  if (state == LexerState::ERROR_STATE) {
    // bad character encountered.
    throw LexerError("Hit lexer error");
  }

  token.type = tokenType(state);
  return token;
}

TokenType tokenType(LexerState finalState) {

  switch (finalState) {
    // literals
  case INTEGER_LITERAL_STATE:
    return INTEGER_LITERAL;
  case FLOAT_LITERAL_STATE:
    return FLOAT_LITERAL;
  case COLOUR_LITERAL_STATE:
    return COLOUR_LITERAL;

  case IDENTIFIER_STATE:
    return IDENTIFIER;

    // operators
  case COMMA_STATE:
    return COMMA_TOK;
  case GREATER_STATE:
    return GREATER_TOK;
  case LESS_STATE:
    return LESS_TOK;
  case EQ_STATE:
    return EQ_TOK;
  case NEQ_STATE:
    return NEQ_TOK;
  case GE_STATE:
    return GE;
  case LE_STATE:
    return LE;
  case PLUS_STATE:
    return PLUS_TOK;
  case MINUS_STATE:
    return MINUS_TOK;
  case STAR_STATE:
    return STAR_TOK;
  case DIV_STATE:
    return DIV_TOK;
  case ASSIGN_STATE:
    return ASSIGN;
  case LBRACKET_STATE:
    return LBRACKET_TOK;
  case RBRACKET_STATE:
    return RBRACKET_TOK;
  case LBRACE_STATE:
    return LBRACE_TOK;
  case RBRACE_STATE:
    return RBRACE_TOK;
  case ARROW_STATE:
    return ARROW;
  case COLON_STATE:
    return COLON_TOK;
  case SEMICOLON_STATE:
    return SEMICOLON_TOK;
  case DOT_STATE:
    return DOT_TOK;
  default:
    throw std::logic_error("Called tokenType() with invalid state " +
                           std::to_string(finalState));
  }
}

CharClass characterClass(char c) {
  switch (c) {
  case '#':
    return HASH;
  case '_':
    return UNDERSCORE;
  case ',':
    return COMMA;
  case '*':
    return STAR;
  case '/':
    return DIV;
  case '+':
    return PLUS;
  case '-':
    return MINUS;
  case '>':
    return GREATER;
  case '<':
    return LESS;
  case '=':
    return EQ;
  case '!':
    return EXCLAMATION;
  case ':':
    return COLON;
  case ';':
    return SEMICOLON;
  case '.':
    return DOT;
  case '(':
    return LBRACKET;
  case ')':
    return RBRACKET;
  case '{':
    return LBRACE;
  case '}':
    return RBRACE;
  default:
    if (isalpha(c)) {
      return ALPHA;
    }
    if (isdigit(c)) {
      return DIGIT;
    }
    throw LexerError("Unrecognized character " + std::string{c});
  }
}

} // end namespace lexer
