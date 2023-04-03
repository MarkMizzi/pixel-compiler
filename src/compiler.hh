#ifndef COMPILER_H_
#define COMPILER_H_

#include "ast.hh"
#include "codegen.hh"
#include "deadcode.hh"
#include "lexer.hh"
#include "parser.hh"
#include "peephole.hh"
#include "semantic_visitor.hh"
#include "util.hh"
#include "xml_visitor.hh"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

struct CompilerOptions {
  std::optional<std::string> outfile = std::nullopt;
  std::optional<std::string> infile = std::nullopt;

  bool generateXml = false;
  std::optional<std::string> xmlOutfile = std::nullopt;

  bool eliminateDeadCode = false;
  bool peepholeOptimize = false;
};

class Compiler {
private:
  // an ugly hack that allows us to use both fstreams and iostreams for
  // stdout/stdin as needed. We use the references in, out, xmlOut in the
  // compile() method, but have these attributes in case we want to do file I/O
  // instead of stdout/stdin.
  std::fstream infile;
  std::fstream outfile;
  std::fstream xmlOutfile;

  std::istream &in;
  std::ostream &out;
  std::ostream &xmlOut;

  lexer::Lexer lexer;
  parser::Parser parser;
  ast::XMLVisitor xmlVisitor;
  ast::SymbolTable symbolTable;
  ast::SemanticVisitor semanticChecker;
  codegen::CodeGenerator codeGenerator;

  CompilerOptions opts;

public:
  Compiler(CompilerOptions &opts)
      : opts(opts),
        infile(opts.infile ? std::fstream{opts.infile.value(), std::fstream::in}
                           : std::fstream()),
        in(opts.infile ? infile : std::cin),
        outfile(opts.outfile
                    ? std::fstream{opts.outfile.value(),
                                   std::fstream::out | std::fstream::trunc}
                    : std::fstream()),
        out(opts.outfile ? outfile : std::cout),
        xmlOutfile(opts.generateXml && opts.xmlOutfile
                       ? std::fstream{opts.xmlOutfile.value(),
                                      std::fstream::out | std::fstream::trunc}
                       : std::fstream()),
        xmlOut(opts.xmlOutfile ? xmlOutfile : std::cout), lexer(in),
        parser(lexer), semanticChecker(symbolTable),
        codeGenerator(symbolTable) {
    if (opts.infile && !std::filesystem::exists(opts.infile.value())) {
      throw CompilationError("Input file " + opts.infile.value() +
                             " does not exist.");
    }
  }

  void compile() {
    std::unique_ptr<ast::TranslationUnit> tu{parser.parse()};
    semanticChecker.visit(*tu);

    if (opts.generateXml) {
      xmlVisitor.visit(*tu);
      xmlOut << xmlVisitor.xml();
    }

    codeGenerator.visit(*tu);
    codegen::PixIRCode &code(codeGenerator.code());

    // optimizations
    if (opts.eliminateDeadCode) {
      codegen::DeadFunctionEliminator eliminator(code);
      eliminator.eliminate();
      codegen::eliminateDeadCodeAfterReturn(code);
    }

    if (opts.peepholeOptimize) {
      peepholeOptimize(code);
    }

    codegen::linearizeCode(code);
    codegen::dumpCode(code, out);
  }
};

#endif // COMPILER_H_
