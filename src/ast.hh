#ifndef AST_H_
#define AST_H_

#include "location.hh"
#include "visitor.hh"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace ast {

// since the number of types is finite, we can represent them using an enum,
// rather than an AST node.
enum Typename {
  FLOAT,
  INT,
  BOOL,
  COLOUR,
};

std::string to_string(Typename type);

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

  BinaryOp op;
  ExprNodePtr left, right;

  BinaryExprNode(BinaryOp op, ExprNodePtr &&left, ExprNodePtr &&right,
                 Location loc)
      : ExprNode(loc), op(op), left(std::move(left)), right(std::move(right)) {}

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

  UnaryOp op;
  ExprNodePtr operand;

  UnaryExprNode(UnaryOp op, ExprNodePtr &&operand, Location loc)
      : ExprNode(loc), op(op), operand(std::move(operand)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {operand.get()}; };
};

class FunctionCallNode : public ExprNode {
public:
  std::string funcName;
  std::vector<ExprNodePtr> args;

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
public:
  std::string id;

  IdExprNode(std::string &id, Location loc) : ExprNode(loc), id(id) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class BoolLiteralExprNode : public ExprNode {
public:
  bool x;

  BoolLiteralExprNode(bool x, Location loc) : ExprNode(loc), x(x) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class IntLiteralExprNode : public ExprNode {
public:
  int x;

  IntLiteralExprNode(int x, Location loc) : ExprNode(loc), x(x) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class FloatLiteralExprNode : public ExprNode {
public:
  float x;

  FloatLiteralExprNode(float x, Location loc) : ExprNode(loc), x(x) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };
};

class ColourLiteralExprNode : public ExprNode {
public:
  unsigned char colour;

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
public:
  ExprNodePtr x, y;

  ReadExprNode(ExprNodePtr &&x, ExprNodePtr &&y, Location loc)
      : ExprNode(loc), x(std::move(x)), y(std::move(y)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {x.get(), y.get()}; };
};

class RandiExprNode : public ExprNode {
public:
  ExprNodePtr operand;

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
public:
  std::string id;
  ExprNodePtr expr;

  AssignmentStmt(std::string &id, ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), id(id), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {expr.get()}; };
};

class VariableDeclStmt : public StmtNode {
public:
  std::string id;
  Typename type;
  ExprNodePtr initExpr;

  VariableDeclStmt(std::string &id, Typename type, ExprNodePtr &&initExpr,
                   Location loc)
      : StmtNode(loc), id(id), type(type), initExpr(std::move(initExpr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {initExpr.get()}; };
};

class PrintStmt : public StmtNode {
public:
  ExprNodePtr expr;

  PrintStmt(ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {expr.get()}; };
};

class DelayStmt : public StmtNode {
public:
  ExprNodePtr expr;

  DelayStmt(ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {expr.get()}; };
};

class PixelStmt : public StmtNode {
public:
  ExprNodePtr x, y;
  ExprNodePtr colour;

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
public:
  ExprNodePtr x, y;
  ExprNodePtr w, h;
  ExprNodePtr colour;

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
public:
  ExprNodePtr expr;

  ReturnStmt(ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {expr.get()}; };
};

class IfElseStmt : public StmtNode {
public:
  ExprNodePtr cond;
  StmtNodePtr ifBody;
  StmtNodePtr elseBody;

  IfElseStmt(ExprNodePtr &&cond, StmtNodePtr &&ifBody, StmtNodePtr &&elseBody,
             Location loc)
      : StmtNode(loc), cond(std::move(cond)), ifBody(std::move(ifBody)),
        elseBody(std::move(elseBody)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    std::vector<ASTNode *> children{cond.get(), ifBody.get()};
    if (elseBody != nullptr) {
      children.push_back(elseBody.get());
    }
    return children;
  }
};

class ForStmt : public StmtNode {
public:
  StmtNodePtr varDecl;
  ExprNodePtr cond;
  StmtNodePtr assignment;
  StmtNodePtr body;

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
public:
  ExprNodePtr cond;
  StmtNodePtr body;

  WhileStmt(ExprNodePtr &&cond, StmtNodePtr &&body, Location loc)
      : StmtNode(loc), cond(std::move(cond)), body(std::move(body)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    return {cond.get(), body.get()};
  }
};

using FormalParam = std::pair<std::string, Typename>;

class FuncDeclStmt : public StmtNode {
public:
  std::string funcName;
  std::vector<FormalParam> params;
  Typename retType;
  StmtNodePtr body;

  FuncDeclStmt(std::string &funcName, std::vector<FormalParam> &&params,
               Typename retType, StmtNodePtr &&body, Location loc)
      : StmtNode(loc), funcName(funcName), params(std::move(params)),
        retType(retType), body(std::move(body)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {body.get()}; }
};

class BlockStmt : public StmtNode {
public:
  std::vector<StmtNodePtr> stmts;

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

class TranslationUnit : public StmtNode {
public:
  std::vector<StmtNodePtr> stmts;

  TranslationUnit(std::vector<StmtNodePtr> &&stmts, Location loc)
      : StmtNode(loc), stmts(std::move(stmts)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    std::vector<ASTNode *> children(stmts.size());
    std::transform(stmts.begin(), stmts.end(), children.begin(),
                   [](StmtNodePtr &stmt) { return stmt.get(); });
    return children;
  }
};

} // namespace ast

#endif // AST_H_
