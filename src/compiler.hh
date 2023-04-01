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

  const codegen::PixIRCode compile() {
    std::unique_ptr<ast::TranslationUnit> tu{parser.parse()};
    semanticChecker.visit(*tu);
    codeGenerator.visit(*tu);
    codegen::PixIRCode &code(codeGenerator.code());
    codegen::linearizeCode(code);
    return std::move(code);
  }
};

#endif // COMPILER_H_
