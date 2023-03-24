#include "lexer.hh"

#include <cctype>
#include <map>
#include <string>
#include <utility>

namespace lexer {

enum LexerState {
  START,
};

// the lexer's transition table
static const std::map<std::pair<LexerState, CharClass>, LexerState> tt{
    {},
};

Token GetNextToken() {}

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
