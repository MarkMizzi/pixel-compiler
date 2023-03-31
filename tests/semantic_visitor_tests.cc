#include "ast.hh"
#include "catch2/catch_test_macros.hpp"
#include "lexer.hh"
#include "parser.hh"
#include "semantic_visitor.hh"

#include <catch2/catch_all.hpp>

#include <sstream>

#define TEST_SETUP(INPUT)                                                      \
  std::stringstream ss{(INPUT)};                                               \
  ss.seekp(0);                                                                 \
  lexer::Lexer lexer{ss};                                                      \
  parser::Parser parser{lexer};                                                \
  ast::SymbolTable symbolTable;                                                \
  ast::SemanticVisitor v{symbolTable};                                         \
  std::unique_ptr<ast::TranslationUnit> tu = parser.parse()

// semantic type checks for initialization

TEST_CASE("Semantic check for var declared float initialized with int works.",
          "[semantic]") {
  TEST_SETUP("let t0: float = 3;");
  REQUIRE_THROWS_AS(v.visit(*tu), ast::SemanticError);
}

TEST_CASE("Semantic check for var declared int initialized with float works.",
          "[semantic]") {
  TEST_SETUP("let t0: int = 3.0;");
  REQUIRE_THROWS_AS(v.visit(*tu), ast::SemanticError);
}

TEST_CASE(
    "Semantic check for var declared float initialized with colour works.",
    "[semantic]") {
  TEST_SETUP("let t0: float = #00ff00;");
  REQUIRE_THROWS_AS(v.visit(*tu), ast::SemanticError);
}

// other semantic checks for initialization

TEST_CASE("Semantic check for double-definition of vars works.", "[semantic]") {
  TEST_SETUP("let t0: float = 0.0; let t0: int = 3;");
  REQUIRE_THROWS_AS(v.visit(*tu), ast::SemanticError);
}

// semantic type checks for assignment

TEST_CASE("Semantic check for var declared float assigned with int works.",
          "[semantic]") {
  TEST_SETUP("let t0: float = 3.0; t0 = 4;");
  REQUIRE_THROWS_AS(v.visit(*tu), ast::SemanticError);
}

TEST_CASE("Semantic check for var declared int assigned with float works.",
          "[semantic]") {
  TEST_SETUP("let t0: int = 3; t0 = 4.0;");
  REQUIRE_THROWS_AS(v.visit(*tu), ast::SemanticError);
}

TEST_CASE("Semantic check for var declared float assigned with colour works.",
          "[semantic]") {
  TEST_SETUP("let t0: float = 3.0; t0 = #00ff00;");
  REQUIRE_THROWS_AS(v.visit(*tu), ast::SemanticError);
}

// other semantic checks for assignment

TEST_CASE("Semantic check for undeclared var assigned works.", "[semantic]") {
  TEST_SETUP("t0 = 3;");
  REQUIRE_THROWS_AS(v.visit(*tu), ast::SemanticError);
}

// semantic type checks for binary operators.

TEST_CASE("Semantic check for int + float works.", "[semantic]") {

  TEST_SETUP("let t0: int = 3; let t1 : float = 4.0; let t2: float = t0 + t1;");
  REQUIRE_THROWS_AS(v.visit(*tu), ast::SemanticError);
}

TEST_CASE("Semantic check for float + int works.", "[semantic]") {

  TEST_SETUP("let t0: int = 3; let t1 : float = 4.0; let t2: float = t1 + t0;");
  REQUIRE_THROWS_AS(v.visit(*tu), ast::SemanticError);
}
