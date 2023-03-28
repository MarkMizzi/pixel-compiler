#include "parser.hh"
#include "ast.hh"
#include "lexer.hh"

#include <stdexcept>

namespace parser {

#define CHECK_TOKEN(TOK, TYPE)                                                 \
  if ((TOK).type != TYPE) {                                                    \
    throw ParserError(std::string("Expected ") + #TYPE +                       \
                          ", found invalid token.",                            \
                      (TOK).loc);                                              \
  }

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
    return std::make_unique<ReadExprNode>(std::move(xExpr), std::move(yExpr),
                                          tok.loc.merge(yExpr->loc));
  }
  case lexer::PAD_HEIGHT:
    return std::make_unique<PadHeightExprNode>(consume().loc);
  case lexer::PAD_WIDTH:
    return std::make_unique<PadWidthExprNode>(consume().loc);
  default:
    throw ParserError("Failed in parseFactor", consume().loc);
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

Typename Parser::parseTypename() {
  lexer::Token tok = consume();

  switch (tok.type) {
  case lexer::INT:
    return Typename::INT;
  case lexer::FLOAT:
    return Typename::FLOAT;
  case lexer::COLOUR:
    return Typename::COLOUR;
  case lexer::BOOL:
    return Typename::BOOL;
  default:
    throw ParserError("Expected typename, found invalid token", tok.loc);
  }
}

StmtNodePtr Parser::parseVariableDecl() {
  Location loc = consume().loc; // consume let token;

  lexer::Token iden = consume();
  CHECK_TOKEN(iden, lexer::IDENTIFIER);

  lexer::Token colon = consume();
  CHECK_TOKEN(colon, lexer::COLON_TOK);

  Typename type = parseTypename();

  lexer::Token eqToken = consume();
  CHECK_TOKEN(eqToken, lexer::EQ_TOK);

  ExprNodePtr expr = parseExpr();

  lexer::Token semicolon = consume();
  CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

  loc = loc.merge(semicolon.loc);

  return std::make_unique<VariableDeclStmt>(iden.value, type, std::move(expr),
                                            loc);
}

StmtNodePtr Parser::parseAssignment() {
  lexer::Token iden = consume();

  lexer::Token eqToken = consume();
  CHECK_TOKEN(eqToken, lexer::EQ_TOK);

  ExprNodePtr expr = parseExpr();

  lexer::Token semicolon = consume();
  CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

  return std::make_unique<AssignmentStmt>(iden.value, std::move(expr),
                                          iden.loc.merge(semicolon.loc));
}

StmtNodePtr Parser::parsePrint() {
  Location loc = consume().loc;

  ExprNodePtr expr = parseExpr();

  lexer::Token semicolon = consume();
  CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

  return std::make_unique<PrintStmt>(std::move(expr), loc.merge(semicolon.loc));
}

StmtNodePtr Parser::parseDelay() {
  Location loc = consume().loc;

  ExprNodePtr expr = parseExpr();

  lexer::Token semicolon = consume();
  CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

  return std::make_unique<DelayStmt>(std::move(expr), loc.merge(semicolon.loc));
}

StmtNodePtr Parser::parsePixelR() {
  lexer::Token comma;
  Location loc = consume().loc;

  ExprNodePtr xExpr = parseExpr();

  comma = consume();
  CHECK_TOKEN(comma, lexer::COMMA_TOK);

  ExprNodePtr yExpr = parseExpr();

  comma = consume();
  CHECK_TOKEN(comma, lexer::COMMA_TOK);

  ExprNodePtr wExpr = parseExpr();

  comma = consume();
  CHECK_TOKEN(comma, lexer::COMMA_TOK);

  ExprNodePtr hExpr = parseExpr();

  comma = consume();
  CHECK_TOKEN(comma, lexer::COMMA_TOK);

  ExprNodePtr expr = parseExpr();

  comma = consume();
  CHECK_TOKEN(comma, lexer::COMMA_TOK);

  lexer::Token semicolon = consume();
  CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

  return std::make_unique<PixelRStmt>(
      std::move(xExpr), std::move(yExpr), std::move(wExpr), std::move(hExpr),
      std::move(expr), loc.merge(semicolon.loc));
}

StmtNodePtr Parser::parseStatement() {
  switch (peek(0).type) {
  case lexer::LET:
    return parseVariableDecl();
  case lexer::IDENTIFIER:
    return parseAssignment();
  case lexer::PRINT:
    return parsePrint();
  case lexer::DELAY:
    return parseDelay();
  case lexer::PIXEL:

  case lexer::PIXELR:
    return parsePixelR();
  case lexer::IF:
  case lexer::FOR:
  case lexer::WHILE:
  case lexer::RETURN:
  case lexer::FUN:
  case lexer::LBRACE_TOK:
  default:
    throw ParserError("Failed in parseStmt", consume().loc);
  }
}

} // namespace parser
