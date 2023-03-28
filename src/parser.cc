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

ExprNodePtr Parser::parseFactor() {
  switch (peek(0).type) {
  case lexer::INTEGER_LITERAL: {
    lexer::Token tok = consume();
    return std::make_unique<IntLiteralExprNode>(std::stoi(tok.value), tok.loc);
  }
  case lexer::FLOAT_LITERAL: {
    lexer::Token tok = consume();
    return std::make_unique<FloatLiteralExprNode>(std::stof(tok.value),
                                                  tok.loc);
  }
  case lexer::TRUE_LITERAL:
    return std::make_unique<BoolLiteralExprNode>(true, consume().loc);
  case lexer::FALSE_LITERAL:
    return std::make_unique<BoolLiteralExprNode>(false, consume().loc);
  case lexer::COLOUR_LITERAL: {
    lexer::Token tok = consume();
    return std::make_unique<ColourLiteralExprNode>(
        std::stoi(tok.value.substr(0), nullptr, 16), tok.loc);
  }
  case lexer::LBRACKET_TOK: {
    consume(); // consume (
    ExprNodePtr subexpr = parseExpr();
    if (consume().type != lexer::RBRACKET_TOK) {
      // consume )
      throw ParserError("Mismatched bracket", loc);
    }
    return subexpr;
  }
  case lexer::MINUS_TOK: {
    lexer::Token tok = consume();
    ExprNodePtr subexpr = parseExpr();
    return std::make_unique<UnaryExprNode>(UnaryExprNode::UnaryOp::MINUS,
                                           std::move(subexpr),
                                           tok.loc.merge(subexpr->loc));
  }
  case lexer::NOT: {
    lexer::Token tok = consume();
    ExprNodePtr subexpr = parseExpr();
    return std::make_unique<UnaryExprNode>(UnaryExprNode::UnaryOp::NOT,
                                           std::move(subexpr),
                                           tok.loc.merge(subexpr->loc));
  }
  case lexer::RANDI: {
    lexer::Token tok = consume();
    ExprNodePtr subexpr = parseExpr();
    return std::make_unique<RandiExprNode>(std::move(subexpr),
                                           tok.loc.merge(subexpr->loc));
  }
  case lexer::READ: {
    lexer::Token tok = consume();
    ExprNodePtr xExpr = parseExpr();
    ExprNodePtr yExpr = parseExpr();
    return std::make_unique<ReadExprNode>(
        std::move(xExpr), std::move(yExpr),
        tok.loc.merge(xExpr->loc).merge(yExpr->loc));
  }
  case lexer::PAD_HEIGHT:
    return std::make_unique<PadHeightExprNode>(consume().loc);
  case lexer::PAD_WIDTH:
    return std::make_unique<PadWidthExprNode>(consume().loc);
  default:
    throw ParserError("Failed in parseFactor", loc);
  }
}

ExprNodePtr Parser::parseTerm() {
  ExprNodePtr left = parseFactor();

  switch (peek(0).type) {
  case lexer::STAR_TOK:
  case lexer::DIV_TOK:
  case lexer::AND: {
    consume();

    BinaryExprNode::BinaryOp op = tokenTypeToBinaryOp(consume().type);
    ExprNodePtr right = parseTerm();

    return std::make_unique<BinaryExprNode>(
        op, std::move(left), std::move(right), left->loc.merge(right->loc));
  }
  default:
    return left;
  }
}

ExprNodePtr Parser::parseSimpleExpr() {
  ExprNodePtr left = parseTerm();

  switch (peek(0).type) {
  case lexer::PLUS_TOK:
  case lexer::MINUS_TOK:
  case lexer::OR: {
    consume();

    BinaryExprNode::BinaryOp op = tokenTypeToBinaryOp(consume().type);
    ExprNodePtr right = parseSimpleExpr();

    return std::make_unique<BinaryExprNode>(
        op, std::move(left), std::move(right), left->loc.merge(right->loc));
  }
  default:
    return left;
  }
}

ExprNodePtr Parser::parseExpr() {
  ExprNodePtr left = parseSimpleExpr();

  switch (peek(0).type) {
  case lexer::GREATER_TOK:
  case lexer::LESS_TOK:
  case lexer::EQ_TOK:
  case lexer::NEQ_TOK:
  case lexer::GE:
  case lexer::LE: {
    consume();

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
