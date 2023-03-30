#ifndef COMPILER_H_
#define COMPILER_H_

#include "ast.hh"
#include "lexer.hh"
#include "parser.hh"
#include "semantic_visitor.hh"

#include <fstream>
#include <memory>
#include <string>

class Compiler {
private:
  std::fstream fs;
  lexer::Lexer lexer;
  parser::Parser parser;
  ast::SemanticVisitor semanticChecker;

public:
  Compiler(std::string fname)
      : fs(fname, fs.in), lexer(lexer::Lexer(fs)), parser(lexer) {}

  void compile() {
    std::unique_ptr<ast::TranslationUnit> tu = parser.parse();
    semanticChecker.visit(*tu);
  }
};

#endif // COMPILER_H_
