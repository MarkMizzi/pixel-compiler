#ifndef VISITOR_H_
#define VISITOR_H_

namespace parser {

class BinaryExprNode;
class UnaryExprNode;
class FunctionCallNode;
class IdExprNode;
class BoolLiteralExprNode;
class IntLiteralExprNode;
class FloatLiteralExprNode;
class ColourLiteralExprNode;
class PadWidthExprNode;
class PadHeightExprNode;
class ReadExprNode;
class RandiExprNode;

class AbstractVisitor {
public:
  virtual void visit(BinaryExprNode &node) = 0;
  virtual void visit(UnaryExprNode &node) = 0;
  virtual void visit(FunctionCallNode &node) = 0;
  virtual void visit(IdExprNode &node) = 0;
  virtual void visit(BoolLiteralExprNode &node) = 0;
  virtual void visit(IntLiteralExprNode &node) = 0;
  virtual void visit(FloatLiteralExprNode &node) = 0;
  virtual void visit(ColourLiteralExprNode &node) = 0;
  virtual void visit(PadWidthExprNode &node) = 0;
  virtual void visit(PadHeightExprNode &node) = 0;
  virtual void visit(ReadExprNode &node) = 0;
  virtual void visit(RandiExprNode &node) = 0;
};

} // namespace parser

#endif // VISITOR_H_
