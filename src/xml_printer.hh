#ifndef XML_PRINTER_H_
#define XML_PRINTER_H_

#include "ast.hh"
#include "lexer.hh"
#include "parser.hh"
#include "xml_visitor.hh"

#include <fstream>
#include <memory>
#include <string>

class XMLPrinter {
private:
  std::fstream fs;
  lexer::Lexer lexer;
  parser::Parser parser;
  ast::XMLVisitor visitor;

public:
  XMLPrinter(std::string fname)
      : fs(fname, fs.in), lexer(lexer::Lexer(fs)), parser(&lexer) {}

  std::string xml() {
    std::unique_ptr<ast::TranslationUnit> tu = parser.parse();
    visitor.visit(*tu);
    return visitor.xml();
  }
};

#endif // XML_PRINTER_H_
