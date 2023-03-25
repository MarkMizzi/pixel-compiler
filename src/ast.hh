#ifndef AST_H_
#define AST_H_

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

class ASTNode {
public:
  Location loc;
  ASTNode(Location loc) : loc(loc) {}
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
};

class FunctionCallNode : public ExprNode {
private:
  std::string funcName;
  std::vector<ExprNodePtr> args;

public:
  FunctionCallNode(std::string &funcName, std::vector<ExprNodePtr> &&args,
                   Location loc)
      : ExprNode(loc), funcName(funcName), args(std::move(args)) {}
};

class IdExprNode : public ExprNode {
private:
  std::string id;

public:
  IdExprNode(std::string &id, Location loc) : ExprNode(loc), id(id) {}
};

class BoolLiteralExprNode : public ExprNode {
private:
  bool x;

public:
  BoolLiteralExprNode(bool x, Location loc) : ExprNode(loc), x(x) {}
};

class IntLiteralExprNode : public ExprNode {
private:
  int x;

public:
  IntLiteralExprNode(int x, Location loc) : ExprNode(loc), x(x) {}
};

class FloatLiteralExprNode : public ExprNode {
private:
  float x;

public:
  FloatLiteralExprNode(float x, Location loc) : ExprNode(loc), x(x) {}
};

class ColourLiteralExprNode : public ExprNode {
private:
  unsigned char colour;

public:
  ColourLiteralExprNode(unsigned char colour, Location loc)
      : ExprNode(loc), colour(colour) {}
};

class PadWidthExprNode : public ExprNode {
public:
  using ExprNode::ExprNode;
};

class PadHeightExprNode : public ExprNode {
public:
  using ExprNode::ExprNode;
};

class ReadExprNode : public ExprNode {
private:
  ExprNodePtr op1, op2;

public:
  ReadExprNode(ExprNodePtr &&op1, ExprNodePtr &&op2, Location loc)
      : ExprNode(loc), op1(std::move(op1)), op2(std::move(op2)) {}
};

class RandiExprNode : public ExprNode {
private:
  ExprNodePtr op;

public:
  RandiExprNode(ExprNodePtr &&op, Location loc)
      : ExprNode(loc), op(std::move(op)) {}
};

} // namespace parser

#endif // AST_H_
