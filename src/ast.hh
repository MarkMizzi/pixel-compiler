#ifndef AST_H_
#define AST_H_

#include "location.hh"
#include "visitor.hh"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace parser {

// since the number of types is finite, we can represent them using an enum,
// rather than an AST node.
enum Typename {
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
  virtual std::vector<ASTNode *> children() = 0;
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
  std::vector<ASTNode *> children() override {
    return {left.get(), right.get()};
  };
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
  std::vector<ASTNode *> children() override { return {operand.get()}; };
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
  std::vector<ASTNode *> children() override {
    std::vector<ASTNode *> children(args.size());
    std::transform(args.begin(), args.end(), children.begin(),
                   [](ExprNodePtr &arg) { return arg.get(); });
    return children;
  };
};

class IdExprNode : public ExprNode {
private:
  std::string id;

public:
  IdExprNode(std::string &id, Location loc) : ExprNode(loc), id(id) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class BoolLiteralExprNode : public ExprNode {
private:
  bool x;

public:
  BoolLiteralExprNode(bool x, Location loc) : ExprNode(loc), x(x) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class IntLiteralExprNode : public ExprNode {
private:
  int x;

public:
  IntLiteralExprNode(int x, Location loc) : ExprNode(loc), x(x) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class FloatLiteralExprNode : public ExprNode {
private:
  float x;

public:
  FloatLiteralExprNode(float x, Location loc) : ExprNode(loc), x(x) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class ColourLiteralExprNode : public ExprNode {
private:
  unsigned char colour;

public:
  ColourLiteralExprNode(unsigned char colour, Location loc)
      : ExprNode(loc), colour(colour) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class PadWidthExprNode : public ExprNode {
public:
  using ExprNode::ExprNode;

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class PadHeightExprNode : public ExprNode {
public:
  using ExprNode::ExprNode;

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class ReadExprNode : public ExprNode {
private:
  ExprNodePtr x, y;

public:
  ReadExprNode(ExprNodePtr &&x, ExprNodePtr &&y, Location loc)
      : ExprNode(loc), x(std::move(x)), y(std::move(y)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {x.get(), y.get()}; };
};

class RandiExprNode : public ExprNode {
private:
  ExprNodePtr operand;

public:
  RandiExprNode(ExprNodePtr &&operand, Location loc)
      : ExprNode(loc), operand(std::move(operand)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {operand.get()}; };
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
  std::vector<ASTNode *> children() override { return {expr.get()}; };
};

class VariableDeclStmt : public StmtNode {
private:
  std::string id;
  Typename type;
  ExprNodePtr initExpr;

public:
  VariableDeclStmt(std::string &id, Typename type, ExprNodePtr &&initExpr,
                   Location loc)
      : StmtNode(loc), id(id), type(type), initExpr(std::move(initExpr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {initExpr.get()}; };
};

class PrintStmt : public StmtNode {
private:
  ExprNodePtr expr;

public:
  PrintStmt(ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {expr.get()}; };
};

class DelayStmt : public StmtNode {
private:
  ExprNodePtr expr;

public:
  DelayStmt(ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {expr.get()}; };
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

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    return {x.get(), y.get(), colour.get()};
  };
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

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    return {x.get(), y.get(), w.get(), h.get(), colour.get()};
  };
};

class ReturnStmt : public StmtNode {
private:
  ExprNodePtr expr;

public:
  ReturnStmt(ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {expr.get()}; };
};

class IfElseStmt : public StmtNode {
private:
  ExprNodePtr cond;
  StmtNodePtr ifBody;
  StmtNodePtr elseBody;

public:
  IfElseStmt(ExprNodePtr &&cond, StmtNodePtr &&ifBody, StmtNodePtr &&elseBody,
             Location loc)
      : StmtNode(loc), cond(std::move(cond)), ifBody(std::move(ifBody)),
        elseBody(std::move(elseBody)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    return {cond.get(), ifBody.get(), elseBody.get()};
  }
};

class ForStmt : public StmtNode {
private:
  StmtNodePtr varDecl;
  ExprNodePtr cond;
  StmtNodePtr assignment;
  StmtNodePtr body;

public:
  ForStmt(StmtNodePtr &&varDecl, ExprNodePtr &&cond, StmtNodePtr &&assignment,
          StmtNodePtr &&body, Location loc)
      : StmtNode(loc), varDecl(std::move(varDecl)), cond(std::move(cond)),
        assignment(std::move(assignment)), body(std::move(body)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    return {varDecl.get(), cond.get(), assignment.get(), body.get()};
  }
};

class WhileStmt : public StmtNode {
private:
  ExprNodePtr cond;
  StmtNodePtr body;

public:
  WhileStmt(ExprNodePtr &&cond, StmtNodePtr &&body, Location loc)
      : StmtNode(loc), cond(std::move(cond)), body(std::move(body)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    return {cond.get(), body.get()};
  }
};

using FormalParam = std::pair<std::string, Typename>;

class FuncDeclStmt : public StmtNode {
private:
  std::string funcName;
  std::vector<FormalParam> params;
  Typename retType;
  StmtNodePtr body;

public:
  FuncDeclStmt(std::string &funcName, std::vector<FormalParam> &&params,
               Typename retType, StmtNodePtr &&body, Location loc)
      : StmtNode(loc), funcName(funcName), params(std::move(params)),
        retType(retType), body(std::move(body)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {body.get()}; }
};

class BlockStmt : public StmtNode {
private:
  std::vector<StmtNodePtr> stmts;

public:
  BlockStmt(std::vector<StmtNodePtr> &&stmts, Location loc)
      : StmtNode(loc), stmts(std::move(stmts)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    std::vector<ASTNode *> children(stmts.size());
    std::transform(stmts.begin(), stmts.end(), children.begin(),
                   [](StmtNodePtr &stmt) { return stmt.get(); });
    return children;
  }
};

} // namespace parser

#endif // AST_H_
