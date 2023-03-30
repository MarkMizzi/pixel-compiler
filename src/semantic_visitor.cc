#include "semantic_visitor.hh"

#include <stdexcept>

namespace ast {

#define CHECK_TYPE(NODEPTR, TYPE)                                              \
  if (typeCheckerTable[NODEPTR] != (TYPE)) {                                   \
    throw SemanticError(std::string("Expected type ") + (TYPE).to_string() +   \
                            ", found invalid type.",                           \
                        (NODEPTR)->loc);                                       \
  }

void SemanticVisitor::visit(BinaryExprNode &node) {
  visitChildren(&node);
  switch (node.op) {
  case BinaryExprNode::BinaryOp::ADD:
  case BinaryExprNode::BinaryOp::SUB:
  case BinaryExprNode::BinaryOp::DIV:
  case BinaryExprNode::BinaryOp::MUL:
  case BinaryExprNode::BinaryOp::AND:
  case BinaryExprNode::BinaryOp::OR:
  case BinaryExprNode::BinaryOp::GREATER:
  case BinaryExprNode::BinaryOp::LESS:
  case BinaryExprNode::BinaryOp::EQ:
  case BinaryExprNode::BinaryOp::NEQ:
  case BinaryExprNode::BinaryOp::GE:
  case BinaryExprNode::BinaryOp::LE:
    break;
  }
}

void SemanticVisitor::visit(UnaryExprNode &node) {}

void SemanticVisitor::visit(FunctionCallNode &node) {}

void SemanticVisitor::visit(IdExprNode &node) {
  std::optional<SymbolTableEntry> entry = currentScope->get(node.id);
  if (!entry.has_value()) {
    throw SemanticError("Symbol " + node.id + " is not in scope.", node.loc);
  }

  typeCheckerTable[&node] = entry.value().type;
}

void SemanticVisitor::visit(BoolLiteralExprNode &node) {
  typeCheckerTable[&node] = SemanticType(Typename::BOOL);
}

void SemanticVisitor::visit(IntLiteralExprNode &node) {
  typeCheckerTable[&node] = SemanticType(Typename::INT);
}

void SemanticVisitor::visit(FloatLiteralExprNode &node) {
  typeCheckerTable[&node] = SemanticType(Typename::FLOAT);
}

void SemanticVisitor::visit(ColourLiteralExprNode &node) {
  typeCheckerTable[&node] = SemanticType(Typename::COLOUR);
}

void SemanticVisitor::visit(PadWidthExprNode &node) {
  typeCheckerTable[&node] = SemanticType(Typename::INT);
}

void SemanticVisitor::visit(PadHeightExprNode &node) {
  typeCheckerTable[&node] = SemanticType(Typename::INT);
}

void SemanticVisitor::visit(ReadExprNode &node) {
  CHECK_TYPE(node.x.get(), SemanticType(Typename::INT));
  CHECK_TYPE(node.y.get(), SemanticType(Typename::INT));

  typeCheckerTable[&node] = SemanticType(Typename::COLOUR);
}

void SemanticVisitor::visit(RandiExprNode &node) {
  CHECK_TYPE(node.operand.get(), SemanticType(Typename::INT));

  typeCheckerTable[&node] = SemanticType(Typename::INT);
}

void SemanticVisitor::visit(AssignmentStmt &node) {
  visitChildren(&node);

  std::optional<SymbolTableEntry> entry = currentScope->get(node.id);
  if (!entry.has_value()) {
    throw SemanticError("Symbol " + node.id + " is not in scope.", node.loc);
  }

  CHECK_TYPE(node.expr.get(), entry.value().type);
}

void SemanticVisitor::visit(VariableDeclStmt &node) {
  visitChildren(&node);

  SemanticType type = SemanticType(node.type);
  CHECK_TYPE(node.initExpr.get(), type);

  currentScope->add(node.id, SymbolTableEntry{type});
}

void SemanticVisitor::visit(PrintStmt &node) {}

void SemanticVisitor::visit(DelayStmt &node) {}

void SemanticVisitor::visit(PixelStmt &node) {}

void SemanticVisitor::visit(PixelRStmt &node) {}

void SemanticVisitor::visit(ReturnStmt &node) {}

void SemanticVisitor::visit(IfElseStmt &node) {}

void SemanticVisitor::visit(ForStmt &node) {}

void SemanticVisitor::visit(WhileStmt &node) {}

void SemanticVisitor::visit(FuncDeclStmt &node) {}

void SemanticVisitor::visit(BlockStmt &node) {}

void SemanticVisitor::visit(TranslationUnit &node) {}

} // end namespace ast
