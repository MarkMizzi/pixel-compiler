#ifndef AST_H_
#define AST_H_

#include "location.hh"
#include "visitor.hh"

#include <algorithm>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

namespace ast {

class ASTNode {
public:
  Location loc;
  ASTNode(Location loc) : loc(loc) {}

  virtual void accept(AbstractVisitor *v) = 0;
  virtual std::vector<ASTNode *> children() = 0;

  virtual ~ASTNode() {}
};

class TypeNode;
using TypeNodePtr = std::unique_ptr<TypeNode>;

class TypeNode : public ASTNode {
public:
  using ASTNode::ASTNode;

  virtual TypeNodePtr copy() const = 0;

  virtual std::string to_string() const = 0;

  bool operator==(const TypeNode &other) const {
    if (typeid(*this) != typeid(other)) {
      return false;
    }
    return this->equals(other);
  }

  bool operator!=(const TypeNode &other) const { return !(*this == other); }

  virtual bool isFuncType() const { return false; }
  virtual bool isArrType() const { return false; }

protected:
  virtual bool equals(const TypeNode &) const = 0;
};

class IntTypeNode : public TypeNode {
public:
  using TypeNode::TypeNode;

  // useful in the type checker
  IntTypeNode() : TypeNode(Location{}) {}

  TypeNodePtr copy() const override {
    return std::make_unique<IntTypeNode>(loc);
  };

  std::string to_string() const override { return "int"; };

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };

protected:
  bool equals(const TypeNode &) const override { return true; }
};

class FloatTypeNode : public TypeNode {
public:
  using TypeNode::TypeNode;

  // useful in the type checker
  FloatTypeNode() : TypeNode(Location{}) {}

  TypeNodePtr copy() const override {
    return std::make_unique<FloatTypeNode>(loc);
  };

  std::string to_string() const override { return "float"; };

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };

protected:
  bool equals(const TypeNode &) const override { return true; }
};

class ColourTypeNode : public TypeNode {
public:
  using TypeNode::TypeNode;

  // useful in the type checker
  ColourTypeNode() : TypeNode(Location{}) {}

  TypeNodePtr copy() const override {
    return std::make_unique<ColourTypeNode>(loc);
  };

  std::string to_string() const override { return "colour"; };

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };

protected:
  bool equals(const TypeNode &) const override { return true; }
};

class BoolTypeNode : public TypeNode {
public:
  using TypeNode::TypeNode;

  // useful in the type checker
  BoolTypeNode() : TypeNode(Location{}) {}

  TypeNodePtr copy() const override {
    return std::make_unique<BoolTypeNode>(loc);
  };

  std::string to_string() const override { return "bool"; };

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {}; };

protected:
  bool equals(const TypeNode &) const override { return true; }
};

class ArrayTypeNode : public TypeNode {
public:
  TypeNodePtr contained;

  ArrayTypeNode(TypeNodePtr &&contained, Location loc)
      : TypeNode(loc), contained(std::move(contained)) {}

  TypeNodePtr copy() const override {
    return std::make_unique<ArrayTypeNode>(contained->copy(), loc);
  };

  std::string to_string() const override {
    return "[]" + contained->to_string();
  };

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {contained.get()}; };

  bool isArrType() const override { return true; }

protected:
  bool equals(const TypeNode &other) const override {
    const ArrayTypeNode *arrTypeOther =
        static_cast<const ArrayTypeNode *>(&other);
    return *this->contained == *arrTypeOther->contained;
  }
};

class FunctionTypeNode : public TypeNode {
public:
  TypeNodePtr retType;
  std::vector<TypeNodePtr> argTypes;

  FunctionTypeNode(TypeNodePtr &&retType, std::vector<TypeNodePtr> &&argTypes,
                   Location loc)
      : TypeNode(loc), retType(std::move(retType)),
        argTypes(std::move(argTypes)) {}

  TypeNodePtr copy() const override {
    std::vector<TypeNodePtr> argTypesCopy(argTypes.size());

    std::transform(argTypes.begin(), argTypes.end(), argTypesCopy.begin(),
                   [](const TypeNodePtr &argType) { return argType->copy(); });

    return std::make_unique<FunctionTypeNode>(retType->copy(),
                                              std::move(argTypesCopy), loc);
  }

  std::string to_string() const override {
    return retType->to_string() + "(" +
           std::accumulate(
               argTypes.begin(), argTypes.end(), std::string(),
               [](const std::string &res, const TypeNodePtr &argType) {
                 return res + argType->to_string();
               }) +
           ")";
  };

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    std::vector<ASTNode *> children(1 + argTypes.size());
    children[0] = retType.get();
    std::transform(argTypes.begin(), argTypes.end(), children.begin() + 1,
                   [](TypeNodePtr &type) { return type.get(); });
    return children;
  };

  bool isFuncType() const override { return true; }

protected:
  bool equals(const TypeNode &other) const override {
    const FunctionTypeNode *funcTypeOther =
        static_cast<const FunctionTypeNode *>(&other);
    if (*this->retType != *funcTypeOther->retType) {
      return false;
    }

    if (argTypes.size() != funcTypeOther->argTypes.size()) {
      return false;
    }

    for (auto it1 = argTypes.cbegin(), it2 = funcTypeOther->argTypes.cbegin();
         it1 != argTypes.cend(); ++it1, ++it2) {
      if (**it1 != **it2) {
        return false;
      }
    }

    return true;
  }
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

class ArrayAccessNode : public ExprNode {
public:
  ExprNodePtr array;
  ExprNodePtr idx;
  bool isLValue;

  ArrayAccessNode(ExprNodePtr &&array, ExprNodePtr &&idx, bool isLValue,
                  Location loc)
      : ExprNode(loc), array(std::move(array)), idx(std::move(idx)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    return {array.get(), idx.get()};
  }
};

class IdExprNode : public ExprNode {
public:
  std::string id;
  bool isLValue;

  IdExprNode(std::string &id, bool isLValue, Location loc)
      : ExprNode(loc), id(id), isLValue(isLValue) {}

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
  unsigned colour;

  ColourLiteralExprNode(unsigned colour, Location loc)
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

class NewArrExprNode : public ExprNode {
public:
  TypeNodePtr ofType;
  ExprNodePtr operand;

  NewArrExprNode(TypeNodePtr &&ofType, ExprNodePtr &&operand, Location loc)
      : ExprNode(loc), ofType(std::move(ofType)), operand(std::move(operand)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {operand.get()}; };
};

class NullArrExprNode : public ExprNode {
public:
  TypeNodePtr ofType;

  NullArrExprNode(TypeNodePtr &&ofType, Location loc)
      : ExprNode(loc), ofType(std::move(ofType)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override { return {ofType.get()}; };
};

class StmtNode : public ASTNode {
public:
  using ASTNode::ASTNode;
};

using StmtNodePtr = std::unique_ptr<StmtNode>;

class AssignmentStmt : public StmtNode {
public:
  ExprNodePtr lvalue;
  ExprNodePtr expr;

  AssignmentStmt(ExprNodePtr &&lvalue, ExprNodePtr &&expr, Location loc)
      : StmtNode(loc), lvalue(std::move(lvalue)), expr(std::move(expr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    return {lvalue.get(), expr.get()};
  };
};

class VariableDeclStmt : public StmtNode {
public:
  std::string id;
  TypeNodePtr type;
  ExprNodePtr initExpr;

  VariableDeclStmt(std::string &id, TypeNodePtr &&type, ExprNodePtr &&initExpr,
                   Location loc)
      : StmtNode(loc), id(id), type(std::move(type)),
        initExpr(std::move(initExpr)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    return {type.get(), initExpr.get()};
  };
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

using FormalParam = std::pair<std::string, TypeNodePtr>;

class FuncDeclStmt : public StmtNode {
public:
  std::string funcName;
  std::vector<FormalParam> params;
  TypeNodePtr retType;
  StmtNodePtr body;

  FuncDeclStmt(std::string &funcName, std::vector<FormalParam> &&params,
               TypeNodePtr &&retType, StmtNodePtr &&body, Location loc)
      : StmtNode(loc), funcName(funcName), params(std::move(params)),
        retType(std::move(retType)), body(std::move(body)) {}

  void accept(AbstractVisitor *v) override { v->visit(*this); }
  std::vector<ASTNode *> children() override {
    std::vector<ASTNode *> children(params.size() + 2);

    std::transform(params.begin(), params.end(), children.begin(),
                   [](auto const &param) { return param.second.get(); });

    *-- --children.end() = retType.get();
    *--children.end() = body.get();

    return children;
  }
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
