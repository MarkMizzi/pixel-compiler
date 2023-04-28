#include "lexer.hh"

#include <cctype>
#include <map>
#include <set>
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
  LSQBRACE_STATE,
  RSQBRACE_STATE,
  ARROW_STATE,
  COLON_STATE,
  SEMICOLON_STATE,

  WHITESPACE_STATE,
  LINE_COMMENT_STATE,

  // intermediate states
  S0,
  S1,

  S2,
  S3,
  S4,
  S5,
  S6,
  S7,

  S8,
  S9,
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
  LSQBRACE,    // [
  RSQBRACE,    // ]
  HEX,         // [a-fA-F]
  ALPHA,       // [g-zG-Z]
  DIGIT,       // [0-9]
  NEWLINE,     // \n
  WHITESPACE,  // [\r\t\v\f]

  UNRECOGNIZED, // error character class.
};

using LexerTransitionTable =
    std::map<std::pair<LexerState, CharClass>, LexerState>;

// return the token type corresponding to the final state of the lexer.
TokenType tokenType(LexerState finalState);

// return the character class of c.
CharClass characterClass(char c);

// the lexer's transition table
static const LexerTransitionTable tt{
    // identifiers
    {{START, ALPHA}, {IDENTIFIER_STATE}},
    {{START, HEX}, {IDENTIFIER_STATE}},
    {{START, UNDERSCORE}, {IDENTIFIER_STATE}},
    {{IDENTIFIER_STATE, ALPHA}, IDENTIFIER_STATE},
    {{IDENTIFIER_STATE, HEX}, IDENTIFIER_STATE},
    {{IDENTIFIER_STATE, DIGIT}, IDENTIFIER_STATE},
    {{IDENTIFIER_STATE, UNDERSCORE}, IDENTIFIER_STATE},

    // integer and float literals
    {{START, DIGIT}, INTEGER_LITERAL_STATE},
    {{INTEGER_LITERAL_STATE, DIGIT}, INTEGER_LITERAL_STATE},
    {{INTEGER_LITERAL_STATE, DOT}, S0},
    {{S0, DIGIT}, FLOAT_LITERAL_STATE},
    {{FLOAT_LITERAL_STATE, DIGIT}, FLOAT_LITERAL_STATE},

    // colour literals
    {{START, HASH}, S2},
    {{S2, HEX}, S3},
    {{S3, HEX}, S4},
    {{S4, HEX}, S5},
    {{S5, HEX}, S6},
    {{S6, HEX}, S7},
    {{S7, HEX}, COLOUR_LITERAL_STATE},
    {{S2, DIGIT}, S3},
    {{S3, DIGIT}, S4},
    {{S4, DIGIT}, S5},
    {{S5, DIGIT}, S6},
    {{S6, DIGIT}, S7},
    {{S7, DIGIT}, COLOUR_LITERAL_STATE},

    // ,
    {{START, COMMA}, COMMA_STATE},

    // =, ==
    {{START, EQ}, ASSIGN_STATE},
    {{ASSIGN_STATE, EQ}, EQ_STATE},

    // !=
    {{START, EXCLAMATION}, S1},
    {{S1, EQ}, NEQ_STATE},

    // >, >=
    {{START, GREATER}, GREATER_STATE},
    {{GREATER_STATE, EQ}, GE_STATE},

    // <, <=
    {{START, LESS}, LESS_STATE},
    {{LESS_STATE, EQ}, LE_STATE},

    // -, ->
    {{START, MINUS}, MINUS_STATE},
    {{MINUS_STATE, GREATER}, ARROW_STATE},

    // +, *, (, ), {, }, [, ] :, ;
    {{START, PLUS}, PLUS_STATE},
    {{START, STAR}, STAR_STATE},
    {{START, LBRACKET}, LBRACKET_STATE},
    {{START, RBRACKET}, RBRACKET_STATE},
    {{START, LBRACE}, LBRACE_STATE},
    {{START, RBRACE}, RBRACE_STATE},
    {{START, LSQBRACE}, LSQBRACE_STATE},
    {{START, RSQBRACE}, RSQBRACE_STATE},
    {{START, COLON}, COLON_STATE},
    {{START, SEMICOLON}, SEMICOLON_STATE},

    // whitespace
    {{START, WHITESPACE}, WHITESPACE_STATE},
    {{WHITESPACE_STATE, WHITESPACE}, WHITESPACE_STATE},
    {{START, NEWLINE}, WHITESPACE_STATE},
    {{WHITESPACE_STATE, NEWLINE}, WHITESPACE_STATE},

    // /, comments
    {{START, DIV}, DIV_STATE},
    // block comments
    {{DIV_STATE, STAR}, S8},
    {{S8, HASH}, S8},
    {{S8, UNDERSCORE}, S8},
    {{S8, COMMA}, S8},
    {{S8, DIV}, S8},
    {{S8, PLUS}, S8},
    {{S8, MINUS}, S8},
    {{S8, GREATER}, S8},
    {{S8, LESS}, S8},
    {{S8, EQ}, S8},
    {{S8, EXCLAMATION}, S8},
    {{S8, COLON}, S8},
    {{S8, SEMICOLON}, S8},
    {{S8, DOT}, S8},
    {{S8, LBRACKET}, S8},
    {{S8, RBRACKET}, S8},
    {{S8, LBRACE}, S8},
    {{S8, RBRACE}, S8},
    {{S8, LSQBRACE}, S8},
    {{S8, RSQBRACE}, S8},
    {{S8, HEX}, S8},
    {{S8, ALPHA}, S8},
    {{S8, DIGIT}, S8},
    {{S8, NEWLINE}, S8},
    {{S8, WHITESPACE}, S8},

    {{S8, STAR}, S9},

    {{S9, HASH}, S8},
    {{S9, UNDERSCORE}, S8},
    {{S9, COMMA}, S8},
    {{S9, STAR}, S8},
    {{S9, PLUS}, S8},
    {{S9, MINUS}, S8},
    {{S9, GREATER}, S8},
    {{S9, LESS}, S8},
    {{S9, EQ}, S8},
    {{S9, EXCLAMATION}, S8},
    {{S9, COLON}, S8},
    {{S9, SEMICOLON}, S8},
    {{S9, DOT}, S8},
    {{S9, LBRACKET}, S8},
    {{S9, RBRACKET}, S8},
    {{S9, LBRACE}, S8},
    {{S9, RBRACE}, S8},
    {{S9, LSQBRACE}, S8},
    {{S9, RSQBRACE}, S8},
    {{S9, HEX}, S8},
    {{S9, ALPHA}, S8},
    {{S9, DIGIT}, S8},
    {{S9, NEWLINE}, S8},
    {{S9, WHITESPACE}, S8},

    {{S9, DIV}, WHITESPACE_STATE},
    // single line comments
    {{DIV_STATE, DIV}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, HASH}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, UNDERSCORE}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, COMMA}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, DIV}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, STAR}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, PLUS}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, MINUS}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, GREATER}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, LESS}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, EQ}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, EXCLAMATION}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, COLON}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, SEMICOLON}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, DOT}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, LBRACKET}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, RBRACKET}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, LBRACE}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, RBRACE}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, LSQBRACE}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, RSQBRACE}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, HEX}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, ALPHA}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, DIGIT}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, WHITESPACE}, LINE_COMMENT_STATE},
    {{LINE_COMMENT_STATE, NEWLINE}, WHITESPACE_STATE},

};

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
    {"__newarr", TokenType::NEWARR},
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

Token Lexer::getNextToken() {
  Token token;
  // filter out whitespace tokens.
  while ((token = nextToken()).type == TokenType::WHITESPACE_TOK)
    ;
  // check if an identifier is actually a keyword.
  if (token.type == TokenType::IDENTIFIER) {
    if (keywords.count(token.value)) {
      token.type = keywords.at(token.value);
    } else if (token.value[0] == '_') {
      // the spec explicitly forbids identifiers that start with an underscore.
      // Some keywords start with an underscore, so we handle this here instead
      // of in the lexer.
      throw LexerError("Identifier cannot start with _.", line, col);
    }
  }
  return token;
}

Token Lexer::nextToken() {
  LexerState state = START;
  Token token;

  token.loc.sline = line;
  token.loc.scol = col;

  char c;
  input.get(c);

  if (input.eof()) {
    token.loc.eline = token.loc.sline;
    token.loc.ecol = token.loc.scol;
    return {TokenType::END, ""};
  }

  while (!input.eof()) {
    CharClass cclass = characterClass(c);

    if (cclass == UNRECOGNIZED) {
      throw LexerError("Unrecognized character " + std::string{c}, line, col);
    }

    if (!tt.count({state, cclass})) {
      // no transition to make
      input.putback(c);
      break;
    }

    state = tt.at({state, cclass});
    token.value.push_back(c);
    if (c == '\n') {
      line++;
      col = 0;
    } else {
      col++;
    }

    input.get(c);
  }

  if (state == LexerState::ERROR_STATE) {
    // bad character encountered.
    throw LexerError("Lexer got into bad state", line, col);
  }

  token.type = tokenType(state);
  token.loc.eline = line;
  token.loc.ecol = col;
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
  case LSQBRACE_STATE:
    return LSQBRACE_TOK;
  case RSQBRACE_STATE:
    return RSQBRACE_TOK;
  case ARROW_STATE:
    return ARROW;
  case COLON_STATE:
    return COLON_TOK;
  case SEMICOLON_STATE:
    return SEMICOLON_TOK;
  case WHITESPACE_STATE:
    return WHITESPACE_TOK;
  case LINE_COMMENT_STATE:
    return WHITESPACE_TOK;
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
  case '[':
    return LSQBRACE;
  case ']':
    return RSQBRACE;
  case '\n':
    return NEWLINE;
  default:
    if (std::set<char>{'a', 'b', 'c', 'd', 'e', 'f', 'A', 'B', 'C', 'D', 'E',
                       'F'}
            .count(c)) {
      return HEX;
    }
    if (isalpha(c)) {
      return ALPHA;
    }
    if (isdigit(c)) {
      return DIGIT;
    }
    if (isspace(c)) {
      return WHITESPACE;
    }
    return UNRECOGNIZED;
  }
}

std::string to_string(TokenType tokType) {
  switch (tokType) {
    // literals
  case TRUE_LITERAL:
    return "true";
  case FALSE_LITERAL:
    return "false";
  case INTEGER_LITERAL:
    return "[0-9]+";
  case FLOAT_LITERAL:
    return "[0-9]+\\.[0-9]+";
  case COLOUR_LITERAL:
    return "#[a-fA-F0-9]{6}";
  case IDENTIFIER:
    return "[a-zA-Z][a-zA-Z_0-9]*";

    // typenames
  case FLOAT:
    return "float";
  case INT:
    return "int";
  case BOOL:
    return "bool";
  case COLOUR:
    return "colour";

    // keywords
  case PAD_WIDTH:
    return "__width";
  case PAD_HEIGHT:
    return "__height";
  case READ:
    return "__read";
  case RANDI:
    return "__randi";
  case LET:
    return "let";
  case PRINT:
    return "__print";
  case DELAY:
    return "__delay";
  case PIXELR:
    return "__pixelr";
  case PIXEL:
    return "__pixel";
  case RETURN:
    return "return";
  case IF:
    return "if";
  case ELSE:
    return "else";
  case FOR:
    return "for";
  case WHILE:
    return "while";
  case FUN:
    return "fun";
  case NEWARR:
    return "__newarr";

    // operators, special characters
  case COMMA_TOK:
    return ",";
  case GREATER_TOK:
    return ">";
  case LESS_TOK:
    return "<";
  case EQ_TOK:
    return "==";
  case NEQ_TOK:
    return "!=";
  case GE:
    return ">=";
  case LE:
    return "<=";
  case PLUS_TOK:
    return "+";
  case MINUS_TOK:
    return "-";
  case STAR_TOK:
    return "*";
  case DIV_TOK:
    return "/";
  case ASSIGN:
    return "=";
  case AND:
    return "and";
  case OR:
    return "or";
  case NOT:
    return "not";
  case LBRACKET_TOK:
    return "(";
  case RBRACKET_TOK:
    return ")";
  case LBRACE_TOK:
    return "{";
  case RBRACE_TOK:
    return "}";
  case LSQBRACE_TOK:
    return "[";
  case RSQBRACE_TOK:
    return "]";
  case ARROW:
    return "->";
  case COLON_TOK:
    return ":";
  case SEMICOLON_TOK:
    return ";";

  case WHITESPACE_TOK:
    return "comments/whitespace";

  case END:
    return "EOF";
  }
  return ""; // please compiler
}

} // end namespace lexer
