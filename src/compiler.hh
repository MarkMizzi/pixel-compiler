#ifndef COMPILER_H_
#define COMPILER_H_

#include "ast.hh"
#include "codegen.hh"
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
  ast::SymbolTable symbolTable;
  ast::SemanticVisitor semanticChecker;
  codegen::CodeGenerator codeGenerator;

public:
  Compiler(std::string fname)
      : fs(fname, fs.in), lexer(lexer::Lexer(fs)), parser(lexer),
        semanticChecker(symbolTable), codeGenerator(symbolTable) {}

  std::vector<codegen::PixIRFunction> compile() {
    std::unique_ptr<ast::TranslationUnit> tu = parser.parse();
    semanticChecker.visit(*tu);
    codeGenerator.visit(*tu);
    return codeGenerator.code();
  }
};

#endif // COMPILER_H_
