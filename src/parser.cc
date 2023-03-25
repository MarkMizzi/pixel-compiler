#include "parser.hh"
#include "ast.hh"
#include "lexer.hh"

#include <stdexcept>

namespace parser {

BinaryExprNode::BinaryOp tokenTypeToBinaryOp(lexer::TokenType tokType) {
  switch (tokType) {
  case lexer::PLUS_TOK:
    return BinaryExprNode::BinaryOp::ADD;
  case lexer::MINUS_TOK:
    return BinaryExprNode::BinaryOp::SUB;
  case lexer::STAR_TOK:
    return BinaryExprNode::BinaryOp::MUL;
  case lexer::DIV_TOK:
    return BinaryExprNode::BinaryOp::DIV;
  case lexer::AND:
    return BinaryExprNode::BinaryOp::AND;
  case lexer::OR:
    return BinaryExprNode::BinaryOp::OR;
  case lexer::GREATER_TOK:
    return BinaryExprNode::BinaryOp::GREATER;
  case lexer::LESS_TOK:
    return BinaryExprNode::BinaryOp::LESS;
  case lexer::EQ_TOK:
    return BinaryExprNode::BinaryOp::EQ;
  case lexer::NEQ_TOK:
    return BinaryExprNode::BinaryOp::NEQ;
  case lexer::GE:
    return BinaryExprNode::BinaryOp::GE;
  case lexer::LE:
    return BinaryExprNode::BinaryOp::LE;
  default:
    throw std::logic_error("Tokens of type " + std::to_string(tokType) +
                           " do not represent a binary operator.");
  }
}

ExprNodePtr Parser::parseFactor() {}

ExprNodePtr Parser::parseTerm() {}

ExprNodePtr Parser::parseSimpleExpr() {}

ExprNodePtr Parser::parseExpr() {
  ExprNodePtr left = parseSimpleExpr();
  switch (peek(0).type) {
  case lexer::GREATER_TOK:
  case lexer::LESS_TOK:
  case lexer::EQ_TOK:
  case lexer::NEQ_TOK:
  case lexer::GE:
  case lexer::LE: {
    BinaryExprNode::BinaryOp op = tokenTypeToBinaryOp(consume().type);
    ExprNodePtr right = parseSimpleExpr();

    return std::make_unique<BinaryExprNode>(
        op, std::move(left), std::move(right), left->loc.merge(right->loc));
  }
  default:
    return left;
  }
}

} // namespace parser
