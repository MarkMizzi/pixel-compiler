#ifndef COMPILER_H_
#define COMPILER_H_

#include "ast.hh"
#include "codegen.hh"
#include "lexer.hh"
#include "parser.hh"
#include "semantic_visitor.hh"
#include "xml_visitor.hh"

#include <iostream>
#include <memory>
#include <string>

struct CompilerOptions {
  bool generateXml;
  std::ostream &xmlStream = std::cout;
};

class Compiler {
private:
  std::ostream &out;
  lexer::Lexer lexer;
  parser::Parser parser;
  ast::XMLVisitor xmlVisitor;
  ast::SymbolTable symbolTable;
  ast::SemanticVisitor semanticChecker;
  codegen::CodeGenerator codeGenerator;

  CompilerOptions opts;

public:
  Compiler(std::istream &in, std::ostream &out, CompilerOptions &opts)
      : out(out), lexer(lexer::Lexer(in)), parser(lexer),
        semanticChecker(symbolTable), codeGenerator(symbolTable), opts(opts) {}

  void compile() {
    std::unique_ptr<ast::TranslationUnit> tu{parser.parse()};
    semanticChecker.visit(*tu);

    if (opts.generateXml) {
      xmlVisitor.visit(*tu);
      opts.xmlStream << xmlVisitor.xml();
    }

    codeGenerator.visit(*tu);
    codegen::PixIRCode &code(codeGenerator.code());
    codegen::linearizeCode(code);
    codegen::dumpCode(code, out);
  }
};

#endif // COMPILER_H_
