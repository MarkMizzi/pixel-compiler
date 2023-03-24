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
  TRUE_STATE,
  FALSE_STATE,
  INTEGER_LITERAL_STATE,
  FLOAT_LITERAL_STATE,
  COLOUR_LITERAL_STATE,

  IDENTIFIER_STATE,

  // typenames
  FLOAT_STATE,
  INT_STATE,
  BOOL_STATE,
  COLOUR_STATE,

  // keywords
  PAD_WIDTH_STATE,
  PAD_HEIGHT_STATE,
  READ_STATE,
  RANDI_STATE,
  LET_STATE,
  PRINT_STATE,
  DELAY_STATE,
  PIXELR_STATE,
  PIXEL_STATE,
  RETURN_STATE,
  IF_STATE,
  ELSE_STATE,
  FOR_STATE,
  WHILE_STATE,
  FUN_STATE,

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
  AND_STATE,
  OR_STATE,
  NOT_STATE,
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

using LexerTransitionTable =
    std::map<std::pair<LexerState, CharClass>, LexerState>;

// return the token type corresponding to the final state of the lexer.
TokenType tokenType(LexerState finalState);

// is this state a final state of the lexer?
bool isFinal(LexerState state);

// return the character class of c.
CharClass characterClass(char c);

// the lexer's transition table
static const LexerTransitionTable tt{{{START, COMMA}, COMMA_STATE}};

Token Lexer::GetNextToken() {
  Token token;
  // filter out whitespace tokens.
  while ((token = nextToken()).type == WHITESPACE)
    ;
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
  case TRUE_STATE:
    return TRUE_LITERAL;
  case FALSE_STATE:
    return FALSE_LITERAL;
  case INTEGER_LITERAL_STATE:
    return INTEGER_LITERAL;
  case FLOAT_LITERAL_STATE:
    return FLOAT_LITERAL;
  case COLOUR_LITERAL_STATE:
    return COLOUR_LITERAL;

  case IDENTIFIER_STATE:
    return IDENTIFIER;

    // typenames
  case FLOAT_STATE:
    return FLOAT;
  case INT_STATE:
    return INT;
  case BOOL_STATE:
    return BOOL;
  case COLOUR_STATE:
    return COLOUR;

    // keywords
  case PAD_WIDTH_STATE:
    return PAD_WIDTH;
  case PAD_HEIGHT_STATE:
    return PAD_HEIGHT;
  case READ_STATE:
    return READ;
  case RANDI_STATE:
    return RANDI;
  case LET_STATE:
    return LET;
  case PRINT_STATE:
    return PRINT;
  case DELAY_STATE:
    return DELAY;
  case PIXELR_STATE:
    return PIXELR;
  case PIXEL_STATE:
    return PIXEL;
  case RETURN_STATE:
    return RETURN;
  case IF_STATE:
    return IF;
  case ELSE_STATE:
    return ELSE;
  case FOR_STATE:
    return FOR;
  case WHILE_STATE:
    return WHILE;
  case FUN_STATE:
    return FUN;

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
  case AND_STATE:
    return AND;
  case OR_STATE:
    return OR;
  case NOT_STATE:
    return NOT;
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

bool isFinal(LexerState state) {

  switch (state) {
    // literals
  case TRUE_STATE:
  case FALSE_STATE:
  case INTEGER_LITERAL_STATE:
  case FLOAT_LITERAL_STATE:
  case COLOUR_LITERAL_STATE:

  case IDENTIFIER_STATE:

    // typenames
  case FLOAT_STATE:
  case INT_STATE:
  case BOOL_STATE:
  case COLOUR_STATE:

    // keywords
  case PAD_WIDTH_STATE:
  case PAD_HEIGHT_STATE:
  case READ_STATE:
  case RANDI_STATE:
  case LET_STATE:
  case PRINT_STATE:
  case DELAY_STATE:
  case PIXELR_STATE:
  case PIXEL_STATE:
  case RETURN_STATE:
  case IF_STATE:
  case ELSE_STATE:
  case FOR_STATE:
  case WHILE_STATE:
  case FUN_STATE:

    // operators
  case COMMA_STATE:
  case GREATER_STATE:
  case LESS_STATE:
  case EQ_STATE:
  case NEQ_STATE:
  case GE_STATE:
  case LE_STATE:
  case PLUS_STATE:
  case MINUS_STATE:
  case STAR_STATE:
  case DIV_STATE:
  case ASSIGN_STATE:
  case AND_STATE:
  case OR_STATE:
  case NOT_STATE:
  case LBRACKET_STATE:
  case RBRACKET_STATE:
  case LBRACE_STATE:
  case RBRACE_STATE:
  case ARROW_STATE:
  case COLON_STATE:
  case SEMICOLON_STATE:
  case DOT_STATE:
    return true;
  default:
    return false;
  }
}

CharClass characterClass(char c) {
  switch (c) {
  case 'a':
    return A;
  case 'b':
    return B;
  case 'c':
    return C;
  case 'd':
    return D;
  case 'e':
    return E;
  case 'f':
    return F;
  case 'g':
    return G;
  case 'h':
    return H;
  case 'i':
    return I;
  case 'l':
    return L;
  case 'n':
    return N;
  case 'o':
    return O;
  case 'p':
    return P;
  case 'r':
    return R;
  case 's':
    return S;
  case 't':
    return T;
  case 'u':
    return U;
  case 'w':
    return W;
  case 'x':
    return X;
  case 'y':
    return Y;
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
