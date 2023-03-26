#ifndef AST_H_
#define AST_H_

#include "visitor.hh"

#include <memory>
#include <string>
#include <vector>

namespace parser {

struct Location {
  size_t sline, scol, eline, ecol;

  Location merge(Location &other) {
    return Location{
        std::min(this->sline, other.sline), std::min(this->scol, other.scol),
        std::max(this->eline, other.eline), std::max(this->ecol, other.ecol)};
  }
};

// since the number of types is finite, we can represent them using an enum,
// rather than an AST node.
enum Type {
  FLOAT,
  INT,
  BOOL,
  COLOUR,
};

class ASTNode {
public:
  Location loc;
  ASTNode(Location loc) : loc(loc) {}

  virtual void accept(AbstractVisitor *v) = 0;
};

class ExprNode : public ASTNode {
public:
  using ASTNode::ASTNode;
};

using ExprNodePtr = std::unique_ptr<ExprNode>;

class BinaryExprNode : public ExprNode {
public:
  enum BinaryOp {
    ADD,
    SUB,
    DIV,
    MUL,
    AND,
    OR,
    GREATER,
    LESS,
    EQ,
    NEQ,
    GE,
    LE,
  };

private:
  BinaryOp operand;
  ExprNodePtr left, right;

public:
  BinaryExprNode(BinaryOp operand, ExprNodePtr &&left, ExprNodePtr &&right,
                 Location loc)
      : ExprNode(loc), operand(operand), left(std::move(left)),
        right(std::move(right)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class UnaryExprNode : public ExprNode {
public:
  enum UnaryOp {
    MINUS,
    NOT,
  };

private:
  UnaryOp op;
  ExprNodePtr operand;

public:
  UnaryExprNode(UnaryOp op, ExprNodePtr &&operand, Location loc)
      : ExprNode(loc), op(op), operand(std::move(operand)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class FunctionCallNode : public ExprNode {
private:
  std::string funcName;
  std::vector<ExprNodePtr> args;

public:
  FunctionCallNode(std::string &funcName, std::vector<ExprNodePtr> &&args,
                   Location loc)
      : ExprNode(loc), funcName(funcName), args(std::move(args)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class IdExprNode : public ExprNode {
private:
  std::string id;

public:
  IdExprNode(std::string &id, Location loc) : ExprNode(loc), id(id) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class BoolLiteralExprNode : public ExprNode {
private:
  bool x;

public:
  BoolLiteralExprNode(bool x, Location loc) : ExprNode(loc), x(x) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class IntLiteralExprNode : public ExprNode {
private:
  int x;

public:
  IntLiteralExprNode(int x, Location loc) : ExprNode(loc), x(x) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class FloatLiteralExprNode : public ExprNode {
private:
  float x;

public:
  FloatLiteralExprNode(float x, Location loc) : ExprNode(loc), x(x) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class ColourLiteralExprNode : public ExprNode {
private:
  unsigned char colour;

public:
  ColourLiteralExprNode(unsigned char colour, Location loc)
      : ExprNode(loc), colour(colour) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class PadWidthExprNode : public ExprNode {
public:
  using ExprNode::ExprNode;

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class PadHeightExprNode : public ExprNode {
public:
  using ExprNode::ExprNode;

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class ReadExprNode : public ExprNode {
private:
  ExprNodePtr x, y;

public:
  ReadExprNode(ExprNodePtr &&x, ExprNodePtr &&y, Location loc)
      : ExprNode(loc), x(std::move(x)), y(std::move(y)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class RandiExprNode : public ExprNode {
private:
  ExprNodePtr op;

public:
  RandiExprNode(ExprNodePtr &&op, Location loc)
      : ExprNode(loc), op(std::move(op)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class StmtNode : public ASTNode {
public:
  using ASTNode::ASTNode;
};

using StmtNodePtr = std::unique_ptr<StmtNode>;

class AssignmentStmt : public StmtNode {
private:
  std::string id;
  ExprNodePtr expr;

public:
  AssignmentStmt(std::string &id, ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), id(id), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class VariableDeclStmt : public StmtNode {
private:
  std::string id;
  Type type;
  ExprNodePtr initExpr;

public:
  VariableDeclStmt(std::string &id, Type type, ExprNodePtr &&initExpr,
                   Location loc)
      : StmtNode(loc), id(id), type(type), initExpr(std::move(initExpr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class PrintStmt : public StmtNode {
private:
  ExprNodePtr expr;

public:
  PrintStmt(ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class DelayStmt : public StmtNode {
private:
  ExprNodePtr expr;

public:
  DelayStmt(ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class PixelStmt : public StmtNode {
private:
  ExprNodePtr x, y;
  ExprNodePtr colour;

public:
  PixelStmt(ExprNodePtr &&x, ExprNodePtr &&y, ExprNodePtr &&colour,
            Location loc)
      : StmtNode(loc), x(std::move(x)), y(std::move(y)),
        colour(std::move(colour)) {}
};

class PixelRStmt : public StmtNode {
private:
  ExprNodePtr x, y;
  ExprNodePtr w, h;
  ExprNodePtr colour;

public:
  PixelRStmt(ExprNodePtr &&x, ExprNodePtr &&y, ExprNodePtr &&w, ExprNodePtr &&h,
             ExprNodePtr &&colour, Location loc)
      : StmtNode(loc), x(std::move(x)), y(std::move(y)), w(std::move(w)),
        h(std::move(h)), colour(std::move(colour)) {}
};

// TODO: __pixelr, __pixel nodes.

class ReturnStmt : public StmtNode {
private:
  ExprNodePtr expr;

public:
  ReturnStmt(ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class IfElseStmt : public StmtNode {
private:
  ExprNodePtr cond;
  std::vector<StmtNodePtr> ifBody;
  std::vector<StmtNodePtr> elseBody;

public:
  IfElseStmt(ExprNodePtr &&cond, std::vector<StmtNodePtr> &&ifBody,
             std::vector<StmtNodePtr> &&elseBody, Location loc)
      : StmtNode(loc), cond(std::move(cond)), ifBody(std::move(ifBody)),
        elseBody(std::move(elseBody)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class ForStmt : public StmtNode {
private:
  ExprNodePtr varDecl;
  ExprNodePtr cond;
  ExprNodePtr assignment;
  std::vector<StmtNodePtr> body;

public:
  ForStmt(std::unique_ptr<VariableDeclStmt> &&varDecl, ExprNodePtr &&cond,
          std::unique_ptr<AssignmentStmt> &&assignment,
          std::vector<StmtNodePtr> &&body, Location loc)
      : StmtNode(loc), varDecl(std::move(varDecl)), cond(std::move(cond)),
        assignment(std::move(assignment)), body(std::move(body)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class WhileStmt : public StmtNode {
private:
  ExprNodePtr cond;
  std::vector<StmtNodePtr> body;

public:
  WhileStmt(ExprNodePtr &&cond, std::vector<StmtNodePtr> &&body, Location loc)
      : StmtNode(loc), cond(std::move(cond)), body(std::move(body)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

using FormalParam = std::pair<std::string, Type>;

class FuncDeclStmt : public StmtNode {
private:
  std::string funcName;
  std::vector<FormalParam> params;
  Type retType;
  std::vector<StmtNodePtr> body;

public:
  FuncDeclStmt(std::string &funcName, std::vector<FormalParam> &params,
               Type retType, std::vector<StmtNodePtr> &&body, Location loc)
      : StmtNode(loc), funcName(funcName), params(params), retType(retType),
        body(std::move(body)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

class BlockStmt : public StmtNode {
private:
  std::vector<StmtNodePtr> body;

public:
  BlockStmt(std::vector<StmtNodePtr> &&body, Location loc)
      : StmtNode(loc), body(std::move(body)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
};

} // namespace parser

#endif // AST_H_
