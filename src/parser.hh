#ifndef PARSER_H_
#define PARSER_H_

#include "ast.hh"
#include "lexer.hh"

#include <deque>

namespace parser {

class Parser {
private:
  lexer::Lexer *lexer;
  std::deque<lexer::Token> lookahead;

  lexer::Token peek(size_t i) {
    if (i > lookahead.size()) {
      lexer::Token tok = lexer->GetNextToken();
      lookahead.push_back(tok);
      return tok;
    }
    return lookahead[i];
  }

  lexer::Token consume() {
    if (lookahead.size()) {
      lexer::Token tok = lookahead.front();
      lookahead.pop_front();
      return tok;
    }
    return lexer->GetNextToken();
  }

public:
  Parser(lexer::Lexer *lexer) : lexer(lexer) {}

  ExprNodePtr parseFactor();
  ExprNodePtr parseTerm();
  ExprNodePtr parseSimpleExpr();
  ExprNodePtr parseExpr();
};
} // namespace parser

#endif // PARSER_H_
