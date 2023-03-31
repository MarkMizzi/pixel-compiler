#ifndef PARSER_H_
#define PARSER_H_

#include "ast.hh"
#include "lexer.hh"
#include "util.hh"

#include <deque>

namespace parser {

class ParserError : public CompilationError {
public:
  ParserError(std::string errmsg, Location loc)
      : CompilationError("Parser error at [" + std::to_string(loc.sline) + ":" +
                         std::to_string(loc.scol) + "]-[" +
                         std::to_string(loc.eline) + ":" +
                         std::to_string(loc.ecol) + "]: " + errmsg) {}
};

class Parser {
private:
  lexer::Lexer &lexer;
  std::deque<lexer::Token> lookahead;

  Location loc;

  lexer::Token peek(size_t i) {
    if (i + 1 > lookahead.size()) {
      lexer::Token tok = lexer.getNextToken();
      lookahead.push_back(tok);
      return tok;
    }
    return lookahead[i];
  }

  lexer::Token consume() {
    lexer::Token tok;
    if (lookahead.size()) {
      tok = lookahead.front();
      lookahead.pop_front();
    } else {
      tok = lexer.getNextToken();
    }
    loc = tok.loc;
    return tok;
  }

public:
  Parser(lexer::Lexer &lexer) : lexer(lexer) {}

  ast::ExprNodePtr parseFunctionCall();
  ast::ExprNodePtr parseFactor();
  ast::ExprNodePtr parseTerm();
  ast::ExprNodePtr parseSimpleExpr();
  ast::ExprNodePtr parseExpr();

  ast::Typename parseTypename();

  ast::FormalParam parseFormalParam();

  ast::StmtNodePtr parseVariableDecl();
  ast::StmtNodePtr parseAssignment();
  ast::StmtNodePtr parsePrint();
  ast::StmtNodePtr parseDelay();
  ast::StmtNodePtr parsePixel();
  ast::StmtNodePtr parsePixelR();
  ast::StmtNodePtr parseIfElse();
  ast::StmtNodePtr parseFor();
  ast::StmtNodePtr parseWhile();
  ast::StmtNodePtr parseReturn();
  ast::StmtNodePtr parseFun();
  ast::StmtNodePtr parseBlock();
  ast::StmtNodePtr parseStatement();

  std::unique_ptr<ast::TranslationUnit> parse();
};
} // namespace parser

#endif // PARSER_H_
