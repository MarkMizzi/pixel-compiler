#include "lexer.hh"

#include <catch2/catch_all.hpp>

#include <string>

#define TEST_KEYWORD_MATCH(MATCH_STR, TOKEN_TYPE)                              \
  TEST_CASE(std::string("Keyword ") + MATCH_STR + " is matched correctly",     \
            "[lexer]") {                                                       \
    lexer::Lexer lexer{MATCH_STR};                                             \
    REQUIRE(lexer.GetNextToken().type == lexer::TokenType::TOKEN_TYPE);        \
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
  lexer::Lexer lexer{">="};
  REQUIRE(lexer.GetNextToken().type == lexer::TokenType::GE);
}

TEST_CASE("Maximal munch for <= works", "[lexer]") {
  lexer::Lexer lexer{"<="};
  REQUIRE(lexer.GetNextToken().type == lexer::TokenType::LE);
}

TEST_CASE("Maximal munch for == works", "[lexer]") {
  lexer::Lexer lexer{"=="};
  REQUIRE(lexer.GetNextToken().type == lexer::TokenType::EQ_TOK);
}

TEST_CASE("Maximal munch for -> works", "[lexer]") {
  lexer::Lexer lexer{"->"};
  REQUIRE(lexer.GetNextToken().type == lexer::TokenType::ARROW);
}

TEST_CASE("Identifiers cannot start with underscore", "[lexer]") {
  lexer::Lexer lexer{"__not_a_keyword"};
  REQUIRE_THROWS_AS(lexer.GetNextToken(), lexer::LexerError);
}

TEST_CASE("Whitespace is filtered out", "[lexer]") {
  lexer::Lexer lexer{"    \n\t\v     \r \t   \v\t"};
  REQUIRE(lexer.GetNextToken().type == lexer::TokenType::END);
}

TEST_CASE("Line comments are filtered out", "[lexer]") {
  lexer::Lexer lexer{"// This is a comment."};
  REQUIRE(lexer.GetNextToken().type == lexer::TokenType::END);
}

TEST_CASE("Multi-line comments are filtered out", "[lexer]") {
  lexer::Lexer lexer{"/* Hello, I am a multi-\n\nline comment \v\t */"};
  REQUIRE(lexer.GetNextToken().type == lexer::TokenType::END);
}
