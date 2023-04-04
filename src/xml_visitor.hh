#ifndef XML_H_
#define XML_H_

#include "visitor.hh"

#include <sstream>
#include <string>

namespace ast {

class XMLVisitor : public AbstractVisitor {
private:
  std::stringstream ss;
  int indent = 0;

public:
  void visit(IntTypeNode &node) override;
  void visit(FloatTypeNode &node) override;
  void visit(ColourTypeNode &node) override;
  void visit(BoolTypeNode &node) override;
  void visit(ArrayTypeNode &node) override;

  void visit(BinaryExprNode &node) override;
  void visit(UnaryExprNode &node) override;
  void visit(FunctionCallNode &node) override;
  void visit(IdExprNode &node) override;
  void visit(BoolLiteralExprNode &node) override;
  void visit(IntLiteralExprNode &node) override;
  void visit(FloatLiteralExprNode &node) override;
  void visit(ColourLiteralExprNode &node) override;
  void visit(PadWidthExprNode &node) override;
  void visit(PadHeightExprNode &node) override;
  void visit(ReadExprNode &node) override;
  void visit(RandiExprNode &node) override;
  void visit(NewArrExprNode &node) override;
  void visit(ArrayAccessNode &node) override;

  void visit(AssignmentStmt &node) override;
  void visit(VariableDeclStmt &node) override;
  void visit(PrintStmt &node) override;
  void visit(DelayStmt &node) override;
  void visit(PixelStmt &node) override;
  void visit(PixelRStmt &node) override;
  void visit(ReturnStmt &node) override;
  void visit(IfElseStmt &node) override;
  void visit(ForStmt &node) override;
  void visit(WhileStmt &node) override;
  void visit(FuncDeclStmt &node) override;
  void visit(BlockStmt &node) override;

  void visit(TranslationUnit &node) override;

  std::string xml() { return ss.str(); };
};

} // namespace ast

#endif // XML_H_
