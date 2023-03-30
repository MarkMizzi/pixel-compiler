#include "lexer.hh"

#include <catch2/catch_all.hpp>

#include <algorithm>
#include <sstream>
#include <string>

#define TEST_KEYWORD_MATCH(MATCH_STR, TOKEN_TYPE)                              \
  TEST_CASE(std::string("Keyword ") + MATCH_STR + " is matched correctly",     \
            "[lexer]") {                                                       \
    std::stringstream ss(MATCH_STR);                                           \
    ss.seekp(0);                                                               \
    lexer::Lexer lexer{ss};                                                    \
    REQUIRE(lexer.getNextToken().type == lexer::TokenType::TOKEN_TYPE);        \
  }

TEST_KEYWORD_MATCH("float", FLOAT)
TEST_KEYWORD_MATCH("int", INT)
TEST_KEYWORD_MATCH("bool", BOOL)
TEST_KEYWORD_MATCH("colour", COLOUR)
TEST_KEYWORD_MATCH("__width", PAD_WIDTH)
TEST_KEYWORD_MATCH("__height", PAD_HEIGHT)
TEST_KEYWORD_MATCH("__read", READ)
TEST_KEYWORD_MATCH("__randi", RANDI)
TEST_KEYWORD_MATCH("let", LET)
TEST_KEYWORD_MATCH("__print", PRINT)
TEST_KEYWORD_MATCH("__delay", DELAY)
TEST_KEYWORD_MATCH("__pixelr", PIXELR)
TEST_KEYWORD_MATCH("__pixel", PIXEL)
TEST_KEYWORD_MATCH("return", RETURN)
TEST_KEYWORD_MATCH("if", IF)
TEST_KEYWORD_MATCH("else", ELSE)
TEST_KEYWORD_MATCH("for", FOR)
TEST_KEYWORD_MATCH("while", WHILE)
TEST_KEYWORD_MATCH("fun", FUN)

TEST_CASE("Maximal munch for >= works", "[lexer]") {
  std::stringstream ss{">="};
  ss.seekp(0);
  lexer::Lexer lexer{ss};
  REQUIRE(lexer.getNextToken().type == lexer::TokenType::GE);
}

TEST_CASE("Maximal munch for <= works", "[lexer]") {
  std::stringstream ss{"<="};
  ss.seekp(0);
  lexer::Lexer lexer{ss};
  REQUIRE(lexer.getNextToken().type == lexer::TokenType::LE);
}

TEST_CASE("Maximal munch for == works", "[lexer]") {
  std::stringstream ss{"=="};
  ss.seekp(0);
  lexer::Lexer lexer{ss};
  REQUIRE(lexer.getNextToken().type == lexer::TokenType::EQ_TOK);
}

TEST_CASE("Maximal munch for -> works", "[lexer]") {
  std::stringstream ss{"->"};
  ss.seekp(0);
  lexer::Lexer lexer{ss};
  REQUIRE(lexer.getNextToken().type == lexer::TokenType::ARROW);
}

TEST_CASE("Identifiers cannot start with underscore", "[lexer]") {
  std::stringstream ss{"__not_a_keyword"};
  ss.seekp(0);
  lexer::Lexer lexer{ss};
  REQUIRE_THROWS_AS(lexer.getNextToken(), lexer::LexerError);
}

TEST_CASE("Whitespace is filtered out", "[lexer]") {
  std::stringstream ss{"    \n\t\v     \r \t   \v\t"};
  ss.seekp(0);
  lexer::Lexer lexer{ss};
  REQUIRE(lexer.getNextToken().type == lexer::TokenType::END);
}

TEST_CASE("Line comments are filtered out", "[lexer]") {
  std::stringstream ss{"// This is a comment."};
  ss.seekp(0);
  lexer::Lexer lexer{ss};
  REQUIRE(lexer.getNextToken().type == lexer::TokenType::END);
}

TEST_CASE("Multi-line comments are filtered out", "[lexer]") {
  std::stringstream ss{"/* Hello, I am a multi-\n\nline comment \v\t */"};
  ss.seekp(0);
  lexer::Lexer lexer{ss};
  REQUIRE(lexer.getNextToken().type == lexer::TokenType::END);
}

TEST_CASE("Ad-hoc lexer test 1", "[lexer]") {
  std::stringstream ss{"fun AverageOfTwo(x: int, y : int) -> float {\n"
                       "    let t0 : int = x + y;\n"
                       "    let t1 : float = t0 / 2 ;\n"
                       "    return t1 ;\n"
                       "}"};
  ss.seekp(0);
  lexer::Lexer lexer{ss};

  std::vector<lexer::Token> toks;
  lexer::Token tok;
  while ((tok = lexer.getNextToken()).type != lexer::END) {
    toks.push_back(tok);
  }

  std::vector<lexer::TokenType> tokTypes(toks.size());
  std::vector<std::string> tokValues(toks.size());

  std::transform(toks.begin(), toks.end(), tokTypes.begin(),
                 [](lexer::Token &tok) { return tok.type; });
  std::transform(toks.begin(), toks.end(), tokValues.begin(),
                 [](lexer::Token &tok) { return tok.value; });

  REQUIRE(tokTypes == std::vector<lexer::TokenType>{lexer::FUN,
                                                    lexer::IDENTIFIER,
                                                    lexer::LBRACKET_TOK,
                                                    lexer::IDENTIFIER,
                                                    lexer::COLON_TOK,
                                                    lexer::INT,
                                                    lexer::COMMA_TOK,
                                                    lexer::IDENTIFIER,
                                                    lexer::COLON_TOK,
                                                    lexer::INT,
                                                    lexer::RBRACKET_TOK,
                                                    lexer::ARROW,
                                                    lexer::FLOAT,
                                                    lexer::LBRACE_TOK,
                                                    lexer::LET,
                                                    lexer::IDENTIFIER,
                                                    lexer::COLON_TOK,
                                                    lexer::INT,
                                                    lexer::ASSIGN,
                                                    lexer::IDENTIFIER,
                                                    lexer::PLUS_TOK,
                                                    lexer::IDENTIFIER,
                                                    lexer::SEMICOLON_TOK,
                                                    lexer::LET,
                                                    lexer::IDENTIFIER,
                                                    lexer::COLON_TOK,
                                                    lexer::FLOAT,
                                                    lexer::ASSIGN,
                                                    lexer::IDENTIFIER,
                                                    lexer::DIV_TOK,
                                                    lexer::INTEGER_LITERAL,
                                                    lexer::SEMICOLON_TOK,
                                                    lexer::RETURN,
                                                    lexer::IDENTIFIER,
                                                    lexer::SEMICOLON_TOK,
                                                    lexer::RBRACE_TOK});

  REQUIRE(tokValues == std::vector<std::string>{"fun",    "AverageOfTwo",
                                                "(",      "x",
                                                ":",      "int",
                                                ",",      "y",
                                                ":",      "int",
                                                ")",      "->",
                                                "float",  "{",
                                                "let",    "t0",
                                                ":",      "int",
                                                "=",      "x",
                                                "+",      "y",
                                                ";",      "let",
                                                "t1",     ":",
                                                "float",  "=",
                                                "t0",     "/",
                                                "2",      ";",
                                                "return", "t1",
                                                ";",      "}"});
}

TEST_CASE("Ad-hoc lexer test 2", "[lexer]") {
  std::stringstream ss{
      "/* Same functionality as function above but using less code.\n"
      " * Note the use of brackets in the expression following the\n"
      " * return statement. Allocates space for 2 variables.\n"
      " */"
      "fun AverageOfTwo2( x : int, y : int ) -> float {"
      "return (x + y) / 2;"
      "}"};
  ss.seekp(0);
  lexer::Lexer lexer{ss};

  std::vector<lexer::Token> toks;
  lexer::Token tok;
  while ((tok = lexer.getNextToken()).type != lexer::END) {
    toks.push_back(tok);
  }

  std::vector<lexer::TokenType> tokTypes(toks.size());
  std::vector<std::string> tokValues(toks.size());

  std::transform(toks.begin(), toks.end(), tokTypes.begin(),
                 [](lexer::Token &tok) { return tok.type; });
  std::transform(toks.begin(), toks.end(), tokValues.begin(),
                 [](lexer::Token &tok) { return tok.value; });

  REQUIRE(tokTypes == std::vector<lexer::TokenType>{
                          lexer::FUN,           lexer::IDENTIFIER,
                          lexer::LBRACKET_TOK,  lexer::IDENTIFIER,
                          lexer::COLON_TOK,     lexer::INT,
                          lexer::COMMA_TOK,     lexer::IDENTIFIER,
                          lexer::COLON_TOK,     lexer::INT,
                          lexer::RBRACKET_TOK,  lexer::ARROW,
                          lexer::FLOAT,         lexer::LBRACE_TOK,
                          lexer::RETURN,        lexer::LBRACKET_TOK,
                          lexer::IDENTIFIER,    lexer::PLUS_TOK,
                          lexer::IDENTIFIER,    lexer::RBRACKET_TOK,
                          lexer::DIV_TOK,       lexer::INTEGER_LITERAL,
                          lexer::SEMICOLON_TOK, lexer::RBRACE_TOK});

  REQUIRE(tokValues == std::vector<std::string>{"fun",    "AverageOfTwo2",
                                                "(",      "x",
                                                ":",      "int",
                                                ",",      "y",
                                                ":",      "int",
                                                ")",      "->",
                                                "float",  "{",
                                                "return", "(",
                                                "x",      "+",
                                                "y",      ")",
                                                "/",      "2",
                                                ";",      "}"});
}

TEST_CASE("Ad-hoc lexer test 3", "[lexer]") {
  std::stringstream ss{
      "// Single line comment\n"
      "fun XGreaterY( x: int, y : int ) -> bool {\n"
      "    let ans: bool = true; if (y > x) { ans = false; } return ans;\n"
      "}"};
  ss.seekp(0);
  lexer::Lexer lexer{ss};

  std::vector<lexer::Token> toks;
  lexer::Token tok;
  while ((tok = lexer.getNextToken()).type != lexer::END) {
    toks.push_back(tok);
  }

  std::vector<lexer::TokenType> tokTypes(toks.size());
  std::vector<std::string> tokValues(toks.size());

  std::transform(toks.begin(), toks.end(), tokTypes.begin(),
                 [](lexer::Token &tok) { return tok.type; });
  std::transform(toks.begin(), toks.end(), tokValues.begin(),
                 [](lexer::Token &tok) { return tok.value; });

  REQUIRE(tokTypes == std::vector<lexer::TokenType>{
                          lexer::FUN,           lexer::IDENTIFIER,
                          lexer::LBRACKET_TOK,  lexer::IDENTIFIER,
                          lexer::COLON_TOK,     lexer::INT,
                          lexer::COMMA_TOK,     lexer::IDENTIFIER,
                          lexer::COLON_TOK,     lexer::INT,
                          lexer::RBRACKET_TOK,  lexer::ARROW,
                          lexer::BOOL,          lexer::LBRACE_TOK,
                          lexer::LET,           lexer::IDENTIFIER,
                          lexer::COLON_TOK,     lexer::BOOL,
                          lexer::ASSIGN,        lexer::TRUE_LITERAL,
                          lexer::SEMICOLON_TOK, lexer::IF,
                          lexer::LBRACKET_TOK,  lexer::IDENTIFIER,
                          lexer::GREATER_TOK,   lexer::IDENTIFIER,
                          lexer::RBRACKET_TOK,  lexer::LBRACE_TOK,
                          lexer::IDENTIFIER,    lexer::ASSIGN,
                          lexer::FALSE_LITERAL, lexer::SEMICOLON_TOK,
                          lexer::RBRACE_TOK,    lexer::RETURN,
                          lexer::IDENTIFIER,    lexer::SEMICOLON_TOK,
                          lexer::RBRACE_TOK});

  REQUIRE(tokValues ==
          std::vector<std::string>{
              "fun", "XGreaterY", "(",   "x",    ":",    "int", ",",     "y",
              ":",   "int",       ")",   "->",   "bool", "{",   "let",   "ans",
              ":",   "bool",      "=",   "true", ";",    "if",  "(",     "y",
              ">",   "x",         ")",   "{",    "ans",  "=",   "false", ";",
              "}",   "return",    "ans", ";",    "}"});
}
