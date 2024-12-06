#include "parser.hh"
#include "ast.hh"
#include "lexer.hh"

#include <stdexcept>

namespace parser
{

#define CHECK_TOKEN(TOK, TYPE)                                                \
  if ((TOK).type != TYPE)                                                     \
  {                                                                           \
    throw ParserError(std::string("Expected \"") + lexer::to_string(TYPE) +   \
                          "\", found invalid token \"" + (TOK).value + "\".", \
                      (TOK).loc);                                             \
  }

  ast::BinaryExprNode::BinaryOp tokenTypeToBinaryOp(lexer::TokenType tokType)
  {
    switch (tokType)
    {
    case lexer::PLUS_TOK:
      return ast::BinaryExprNode::BinaryOp::ADD;
    case lexer::MINUS_TOK:
      return ast::BinaryExprNode::BinaryOp::SUB;
    case lexer::STAR_TOK:
      return ast::BinaryExprNode::BinaryOp::MUL;
    case lexer::DIV_TOK:
      return ast::BinaryExprNode::BinaryOp::DIV;
    case lexer::AND:
      return ast::BinaryExprNode::BinaryOp::AND;
    case lexer::OR:
      return ast::BinaryExprNode::BinaryOp::OR;
    case lexer::GREATER_TOK:
      return ast::BinaryExprNode::BinaryOp::GREATER;
    case lexer::LESS_TOK:
      return ast::BinaryExprNode::BinaryOp::LESS;
    case lexer::EQ_TOK:
      return ast::BinaryExprNode::BinaryOp::EQ;
    case lexer::NEQ_TOK:
      return ast::BinaryExprNode::BinaryOp::NEQ;
    case lexer::GE:
      return ast::BinaryExprNode::BinaryOp::GE;
    case lexer::LE:
      return ast::BinaryExprNode::BinaryOp::LE;
    default:
      throw std::logic_error("Tokens of type " + std::to_string(tokType) +
                             " do not represent a binary operator.");
    }
  }

  ast::ExprNodePtr Parser::parseArrayAccess(bool isLValue)
  {
    lexer::Token iden = consume();

    lexer::Token lsqbrace = consume();
    CHECK_TOKEN(lsqbrace, lexer::LSQBRACE_TOK);

    ast::ExprNodePtr idxExpr = parseExpr();

    lexer::Token rsqbrace = consume();
    CHECK_TOKEN(rsqbrace, lexer::RSQBRACE_TOK);

    // NOTE: Even though we may be using the identifier inside an lvalue, what we
    // really want is the plain value, not the address (index and level) of the
    // identifier, as we are interested in the (constant) address of the head
    // pointer (the value of the identifier).
    ast::ExprNodePtr arrAccess = std::make_unique<ast::ArrayAccessNode>(
        std::make_unique<ast::IdExprNode>(iden.value, false, iden.loc),
        std::move(idxExpr), isLValue, iden.loc.merge(rsqbrace.loc));
    Location loc = arrAccess->loc;

    while (peek(0).type == lexer::LSQBRACE_TOK)
    {
      lsqbrace = consume();
      ast::ExprNodePtr idxExpr = parseExpr();

      rsqbrace = consume();
      CHECK_TOKEN(rsqbrace, lexer::RSQBRACE_TOK);

      loc = loc.merge(rsqbrace.loc);
      arrAccess = std::make_unique<ast::ArrayAccessNode>(
          std::move(arrAccess), std::move(idxExpr), isLValue, loc);
    }

    return arrAccess;
  }

  ast::ExprNodePtr Parser::parseLValueArrayAccess()
  {
    return parseArrayAccess(true);
  }

  ast::ExprNodePtr Parser::parseRValueArrayAccess()
  {
    return parseArrayAccess(false);
  }

  ast::ExprNodePtr Parser::parseFunctionCall()
  {
    lexer::Token funcName = consume();

    lexer::Token lbracket = consume();
    CHECK_TOKEN(lbracket, lexer::LBRACKET_TOK);

    std::vector<ast::ExprNodePtr> args;
    while (peek(0).type != lexer::RBRACKET_TOK)
    {
      args.push_back(std::move(parseExpr()));
      if (peek(0).type != lexer::RBRACKET_TOK)
      {
        consume(); // consume , token
      }
    }

    Location endloc = consume().loc; // consume ) token.

    return std::make_unique<ast::FunctionCallNode>(
        funcName.value, std::move(args), funcName.loc.merge(endloc));
  }

  ast::ExprNodePtr Parser::parseFactor()
  {
    switch (peek(0).type)
    {
    case lexer::INTEGER_LITERAL:
    {
      lexer::Token tok = consume();
      return std::make_unique<ast::IntLiteralExprNode>(std::stoi(tok.value),
                                                       tok.loc);
    }

    case lexer::FLOAT_LITERAL:
    {
      lexer::Token tok = consume();
      return std::make_unique<ast::FloatLiteralExprNode>(std::stof(tok.value),
                                                         tok.loc);
    }

    case lexer::TRUE_LITERAL:
      return std::make_unique<ast::BoolLiteralExprNode>(true, consume().loc);

    case lexer::FALSE_LITERAL:
      return std::make_unique<ast::BoolLiteralExprNode>(false, consume().loc);

    case lexer::COLOUR_LITERAL:
    {
      lexer::Token tok = consume();
      return std::make_unique<ast::ColourLiteralExprNode>(
          std::stoi("0x" + tok.value.substr(1, 6), nullptr, 16), tok.loc);
    }

    case lexer::IDENTIFIER:
    {
      switch (peek(1).type)
      {
      case lexer::LBRACKET_TOK:
        return parseFunctionCall();
      case lexer::LSQBRACE_TOK:
        return parseRValueArrayAccess();
      default:
      {
        lexer::Token tok = consume();
        return std::make_unique<ast::IdExprNode>(tok.value, false, tok.loc);
      }
      }
    }

    case lexer::LBRACKET_TOK:
    {
      consume(); // consume (
      ast::ExprNodePtr subexpr = parseExpr();
      if (consume().type != lexer::RBRACKET_TOK)
      {
        // consume )
        throw ParserError("Mismatched bracket", loc);
      }
      return subexpr;
    }

    case lexer::MINUS_TOK:
    {
      lexer::Token tok = consume();
      ast::ExprNodePtr subexpr = parseFactor();
      return std::make_unique<ast::UnaryExprNode>(
          ast::UnaryExprNode::UnaryOp::MINUS, std::move(subexpr),
          tok.loc.merge(subexpr->loc));
    }

    case lexer::NOT:
    {
      lexer::Token tok = consume();
      ast::ExprNodePtr subexpr = parseFactor();
      return std::make_unique<ast::UnaryExprNode>(
          ast::UnaryExprNode::UnaryOp::NOT, std::move(subexpr),
          tok.loc.merge(subexpr->loc));
    }

    case lexer::RANDI:
    {
      lexer::Token tok = consume();
      ast::ExprNodePtr subexpr = parseExpr();
      return std::make_unique<ast::RandiExprNode>(std::move(subexpr),
                                                  tok.loc.merge(subexpr->loc));
    }

    case lexer::READ:
    {
      lexer::Token tok = consume();

      ast::ExprNodePtr xExpr = parseExpr();

      lexer::Token comma = consume();
      CHECK_TOKEN(comma, lexer::COMMA_TOK);

      ast::ExprNodePtr yExpr = parseExpr();

      return std::make_unique<ast::ReadExprNode>(
          std::move(xExpr), std::move(yExpr), tok.loc.merge(yExpr->loc));
    }

    case lexer::PAD_HEIGHT:
      return std::make_unique<ast::PadHeightExprNode>(consume().loc);

    case lexer::PAD_WIDTH:
      return std::make_unique<ast::PadWidthExprNode>(consume().loc);

    case lexer::NEWARR:
    {
      lexer::Token tok = consume();

      ast::TypeNodePtr ofType = parseType();

      lexer::Token comma = consume();
      CHECK_TOKEN(comma, lexer::COMMA_TOK);

      ast::ExprNodePtr arrSizeExpr = parseExpr();
      Location loc = tok.loc.merge(arrSizeExpr->loc);

      return std::make_unique<ast::NewArrExprNode>(std::move(ofType),
                                                   std::move(arrSizeExpr), loc);
    }

    default:
      throw ParserError("Failed in parseFactor", consume().loc);
    }
  }

  ast::ExprNodePtr Parser::parseTerm()
  {
    ast::ExprNodePtr left = parseFactor();

    switch (peek(0).type)
    {
    case lexer::STAR_TOK:
    case lexer::DIV_TOK:
    case lexer::AND:
    {
      ast::BinaryExprNode::BinaryOp op = tokenTypeToBinaryOp(consume().type);
      ast::ExprNodePtr right = parseTerm();

      return std::make_unique<ast::BinaryExprNode>(
          op, std::move(left), std::move(right), left->loc.merge(right->loc));
    }
    default:
      return left;
    }
  }

  ast::ExprNodePtr Parser::parseSimpleExpr()
  {
    ast::ExprNodePtr left = parseTerm();

    switch (peek(0).type)
    {
    case lexer::PLUS_TOK:
    case lexer::MINUS_TOK:
    case lexer::OR:
    {
      ast::BinaryExprNode::BinaryOp op = tokenTypeToBinaryOp(consume().type);
      ast::ExprNodePtr right = parseSimpleExpr();

      return std::make_unique<ast::BinaryExprNode>(
          op, std::move(left), std::move(right), left->loc.merge(right->loc));
    }
    default:
      return left;
    }
  }

  ast::ExprNodePtr Parser::parseExpr()
  {
    ast::ExprNodePtr left = parseSimpleExpr();

    switch (peek(0).type)
    {
    case lexer::GREATER_TOK:
    case lexer::LESS_TOK:
    case lexer::EQ_TOK:
    case lexer::NEQ_TOK:
    case lexer::GE:
    case lexer::LE:
    {
      ast::BinaryExprNode::BinaryOp op = tokenTypeToBinaryOp(consume().type);
      ast::ExprNodePtr right = parseSimpleExpr();

      return std::make_unique<ast::BinaryExprNode>(
          op, std::move(left), std::move(right), left->loc.merge(right->loc));
    }
    default:
      return left;
    }
  }

  ast::TypeNodePtr Parser::parseType()
  {
    lexer::Token tok = consume();

    switch (tok.type)
    {
    case lexer::INT:
      return std::make_unique<ast::IntTypeNode>(tok.loc);
    case lexer::FLOAT:
      return std::make_unique<ast::FloatTypeNode>(tok.loc);
    case lexer::COLOUR:
      return std::make_unique<ast::ColourTypeNode>(tok.loc);
    case lexer::BOOL:
      return std::make_unique<ast::BoolTypeNode>(tok.loc);
    case lexer::LSQBRACE_TOK:
    {
      lexer::Token rsqbrace = consume();
      CHECK_TOKEN(rsqbrace, lexer::RSQBRACE_TOK);

      ast::TypeNodePtr contained{parseType()};

      Location loc = tok.loc.merge(contained->loc);
      return std::make_unique<ast::ArrayTypeNode>(std::move(contained), loc);
    }
    default:
      throw ParserError("Expected typename, found invalid token", tok.loc);
    }
  }

  ast::StmtNodePtr Parser::parseVariableDecl()
  {
    Location loc = consume().loc; // consume let token;

    lexer::Token iden = consume();
    CHECK_TOKEN(iden, lexer::IDENTIFIER);

    lexer::Token colon = consume();
    CHECK_TOKEN(colon, lexer::COLON_TOK);

    ast::TypeNodePtr type = parseType();

    lexer::Token eqToken = consume();
    CHECK_TOKEN(eqToken, lexer::ASSIGN);

    ast::ExprNodePtr expr = parseExpr();

    lexer::Token semicolon = consume();
    CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

    loc = loc.merge(semicolon.loc);

    return std::make_unique<ast::VariableDeclStmt>(iden.value, std::move(type),
                                                   std::move(expr), loc);
  }

  ast::StmtNodePtr Parser::parseAssignment()
  {

    ast::ExprNodePtr lvalue;

    switch (peek(1).type)
    {
    case lexer::LSQBRACE_TOK:
      lvalue = parseLValueArrayAccess();
      break;
    default:
    {
      lexer::Token tok = consume();
      lvalue = std::make_unique<ast::IdExprNode>(tok.value, true, tok.loc);
    }
    }

    lexer::Token eqToken = consume();
    CHECK_TOKEN(eqToken, lexer::ASSIGN);

    ast::ExprNodePtr expr = parseExpr();

    Location loc = lvalue->loc.merge(expr->loc);

    if (peek(0).type != lexer::RBRACKET_TOK)
    {
      // caters for the case when an assignment is used in a for loop.
      lexer::Token semicolon = consume();
      CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);
      loc.merge(semicolon.loc);
    }

    return std::make_unique<ast::AssignmentStmt>(std::move(lvalue),
                                                 std::move(expr), loc);
  }

  ast::StmtNodePtr Parser::parsePrint()
  {
    Location loc = consume().loc;

    ast::ExprNodePtr expr = parseExpr();

    lexer::Token semicolon = consume();
    CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

    return std::make_unique<ast::PrintStmt>(std::move(expr),
                                            loc.merge(semicolon.loc));
  }

  ast::StmtNodePtr Parser::parseDelay()
  {
    Location loc = consume().loc;

    ast::ExprNodePtr expr = parseExpr();

    lexer::Token semicolon = consume();
    CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

    return std::make_unique<ast::DelayStmt>(std::move(expr),
                                            loc.merge(semicolon.loc));
  }

  ast::StmtNodePtr Parser::parsePixel()
  {
    lexer::Token comma;
    Location loc = consume().loc;

    ast::ExprNodePtr xExpr = parseExpr();

    comma = consume();
    CHECK_TOKEN(comma, lexer::COMMA_TOK);

    ast::ExprNodePtr yExpr = parseExpr();

    comma = consume();
    CHECK_TOKEN(comma, lexer::COMMA_TOK);

    ast::ExprNodePtr expr = parseExpr();

    lexer::Token semicolon = consume();
    CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

    return std::make_unique<ast::PixelStmt>(std::move(xExpr), std::move(yExpr),
                                            std::move(expr),
                                            loc.merge(semicolon.loc));
  }

  ast::StmtNodePtr Parser::parsePixelR()
  {
    lexer::Token comma;
    Location loc = consume().loc;

    ast::ExprNodePtr xExpr = parseExpr();

    comma = consume();
    CHECK_TOKEN(comma, lexer::COMMA_TOK);

    ast::ExprNodePtr yExpr = parseExpr();

    comma = consume();
    CHECK_TOKEN(comma, lexer::COMMA_TOK);

    ast::ExprNodePtr wExpr = parseExpr();

    comma = consume();
    CHECK_TOKEN(comma, lexer::COMMA_TOK);

    ast::ExprNodePtr hExpr = parseExpr();

    comma = consume();
    CHECK_TOKEN(comma, lexer::COMMA_TOK);

    ast::ExprNodePtr expr = parseExpr();

    lexer::Token semicolon = consume();
    CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

    return std::make_unique<ast::PixelRStmt>(
        std::move(xExpr), std::move(yExpr), std::move(wExpr), std::move(hExpr),
        std::move(expr), loc.merge(semicolon.loc));
  }

  ast::StmtNodePtr Parser::parseIfElse()
  {
    Location loc = consume().loc; // consume if token.

    lexer::Token lbracket = consume();
    CHECK_TOKEN(lbracket, lexer::LBRACKET_TOK);

    ast::ExprNodePtr cond = parseExpr();

    lexer::Token rbracket = consume();
    CHECK_TOKEN(rbracket, lexer::RBRACKET_TOK);

    ast::StmtNodePtr ifBody = parseBlock();

    loc = loc.merge(ifBody->loc);

    ast::StmtNodePtr elseBody = nullptr;
    if (peek(0).type == lexer::ELSE)
    {
      consume(); // consume else token
      elseBody = parseBlock();
      loc = loc.merge(elseBody->loc);
    }

    return std::make_unique<ast::IfElseStmt>(std::move(cond), std::move(ifBody),
                                             std::move(elseBody), loc);
  }

  ast::StmtNodePtr Parser::parseFor()
  {
    Location loc = consume().loc; // consume for token.

    lexer::Token lbracket = consume();
    CHECK_TOKEN(lbracket, lexer::LBRACKET_TOK);

    ast::StmtNodePtr varDecl = parseVariableDecl();

    ast::ExprNodePtr cond = parseExpr();

    lexer::Token semicolon = consume();
    CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

    ast::StmtNodePtr assignment = parseAssignment();

    lexer::Token rbracket = consume();
    CHECK_TOKEN(rbracket, lexer::RBRACKET_TOK);

    ast::StmtNodePtr body = parseBlock();

    return std::make_unique<ast::ForStmt>(std::move(varDecl), std::move(cond),
                                          std::move(assignment), std::move(body),
                                          loc.merge(body->loc));
  }

  ast::StmtNodePtr Parser::parseWhile()
  {
    Location loc = consume().loc; // consume while token.

    lexer::Token lbracket = consume();
    CHECK_TOKEN(lbracket, lexer::LBRACKET_TOK);

    ast::ExprNodePtr cond = parseExpr();

    lexer::Token rbracket = consume();
    CHECK_TOKEN(rbracket, lexer::RBRACKET_TOK);

    ast::StmtNodePtr body = parseBlock();

    return std::make_unique<ast::WhileStmt>(std::move(cond), std::move(body),
                                            loc.merge(body->loc));
  }

  ast::StmtNodePtr Parser::parseReturn()
  {
    Location loc = consume().loc;

    ast::ExprNodePtr expr = parseExpr();

    lexer::Token semicolon = consume();
    CHECK_TOKEN(semicolon, lexer::SEMICOLON_TOK);

    return std::make_unique<ast::ReturnStmt>(std::move(expr),
                                             loc.merge(semicolon.loc));
  }

  ast::FormalParam Parser::parseFormalParam()
  {
    lexer::Token iden = consume();
    CHECK_TOKEN(iden, lexer::IDENTIFIER);

    lexer::Token semicolon = consume();
    CHECK_TOKEN(semicolon, lexer::COLON_TOK);

    ast::TypeNodePtr type = parseType();

    return {iden.value, std::move(type)};
  }

  ast::StmtNodePtr Parser::parseFun()
  {
    Location loc = consume().loc; // consume fun token

    lexer::Token iden = consume();
    CHECK_TOKEN(iden, lexer::IDENTIFIER);

    lexer::Token lbracket = consume();
    CHECK_TOKEN(lbracket, lexer::LBRACKET_TOK);

    std::vector<ast::FormalParam> formalParams;

    while (peek(0).type != lexer::RBRACKET_TOK)
    {
      formalParams.push_back(parseFormalParam());
      if (peek(0).type == lexer::COMMA_TOK)
      {
        consume(); // consume , token.
      }
    }

    consume(); // consume ) token

    lexer::Token arrow = consume();
    CHECK_TOKEN(arrow, lexer::ARROW);

    ast::TypeNodePtr type = parseType();

    ast::StmtNodePtr body = parseBlock();

    return std::make_unique<ast::FuncDeclStmt>(
        iden.value, std::move(formalParams), std::move(type), std::move(body),
        loc.merge(body->loc));
  }

  ast::StmtNodePtr Parser::parseBlock()
  {
    Location loc = consume().loc;

    std::vector<ast::StmtNodePtr> stmts;

    while (peek(0).type != lexer::RBRACE_TOK)
    {
      stmts.push_back(std::move(parseStatement()));
    }

    Location endloc = consume().loc; // consume }.

    return std::make_unique<ast::BlockStmt>(std::move(stmts), loc.merge(endloc));
  }

  ast::StmtNodePtr Parser::parseStatement()
  {
    switch (peek(0).type)
    {
    case lexer::LET:
      return parseVariableDecl();
    case lexer::IDENTIFIER:
      return parseAssignment();
    case lexer::PRINT:
      return parsePrint();
    case lexer::DELAY:
      return parseDelay();
    case lexer::PIXEL:
      return parsePixel();
    case lexer::PIXELR:
      return parsePixelR();
    case lexer::IF:
      return parseIfElse();
    case lexer::FOR:
      return parseFor();
    case lexer::WHILE:
      return parseWhile();
    case lexer::RETURN:
      return parseReturn();
    case lexer::FUN:
      return parseFun();
    case lexer::LBRACE_TOK:
      return parseBlock();
    default:
      throw ParserError("Failed in parseStatement", consume().loc);
    }
  }

  std::unique_ptr<ast::TranslationUnit> Parser::parse()
  {
    std::vector<ast::StmtNodePtr> stmts;

    while (peek(0).type != lexer::END)
    {
      stmts.push_back(std::move(parseStatement()));
    }

    consume(); // consume end token

    Location loc = (*stmts.begin())->loc.merge((*--stmts.end())->loc);
    return std::make_unique<ast::TranslationUnit>(std::move(stmts), loc);
  }

} // namespace parser
