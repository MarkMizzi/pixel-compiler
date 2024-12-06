#ifndef VISITOR_H_
#define VISITOR_H_

namespace ast
{

  class ASTNode;

  class IntTypeNode;
  class FloatTypeNode;
  class ColourTypeNode;
  class BoolTypeNode;
  class ArrayTypeNode;
  class FunctionTypeNode;

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
  class NewArrExprNode;
  class ArrayAccessNode;
  class GetCharNode;
  class Float2IntNode;

  class AssignmentStmt;
  class VariableDeclStmt;
  class PrintStmt;
  class DelayStmt;
  class PixelStmt;
  class PixelRStmt;
  class ReturnStmt;
  class PutCharStmt;
  class IfElseStmt;
  class ForStmt;
  class WhileStmt;
  class FuncDeclStmt;
  class BlockStmt;

  class TranslationUnit;

  class AbstractVisitor
  {
  public:
    virtual void visit(IntTypeNode &node) = 0;
    virtual void visit(FloatTypeNode &node) = 0;
    virtual void visit(ColourTypeNode &node) = 0;
    virtual void visit(BoolTypeNode &node) = 0;
    virtual void visit(ArrayTypeNode &node) = 0;
    // optional implementation, as FunctionTypeNode is only involved in semantic
    // checking.
    virtual void visit(FunctionTypeNode &node) {};

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
    virtual void visit(NewArrExprNode &node) = 0;
    virtual void visit(ArrayAccessNode &node) = 0;
    virtual void visit(GetCharNode &node) = 0;
    virtual void visit(Float2IntNode &node) = 0;

    virtual void visit(AssignmentStmt &node) = 0;
    virtual void visit(VariableDeclStmt &node) = 0;
    virtual void visit(PrintStmt &node) = 0;
    virtual void visit(DelayStmt &node) = 0;
    virtual void visit(PixelStmt &node) = 0;
    virtual void visit(PixelRStmt &node) = 0;
    virtual void visit(ReturnStmt &node) = 0;
    virtual void visit(PutCharStmt &node) = 0;
    virtual void visit(IfElseStmt &node) = 0;
    virtual void visit(ForStmt &node) = 0;
    virtual void visit(WhileStmt &node) = 0;
    virtual void visit(FuncDeclStmt &node) = 0;
    virtual void visit(BlockStmt &node) = 0;

    virtual void visit(TranslationUnit &node) = 0;

    void visitChildren(ASTNode *node);
    void rvisitChildren(ASTNode *node);
  };

} // namespace ast

#endif // VISITOR_H_
