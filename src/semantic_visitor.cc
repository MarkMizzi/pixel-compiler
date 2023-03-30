#include "semantic_visitor.hh"

#include <stdexcept>

namespace ast {

SemanticType typenameToSemanticType(Typename type) {
  switch (type) {
  case Typename::INT:
    return SemanticType::INT_TYPE;
  case Typename::BOOL:
    return SemanticType::BOOL_TYPE;
  case Typename::FLOAT:
    return SemanticType::FLOAT_TYPE;
  case Typename::COLOUR:
    return SemanticType::COLOUR_TYPE;
  }
  throw std::logic_error(
      "Impossible, everything was handled in the switch"); // please the
                                                           // compiler
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
  typeCheckerTable[&node] = SemanticType::BOOL_TYPE;
}

void SemanticVisitor::visit(IntLiteralExprNode &node) {
  typeCheckerTable[&node] = SemanticType::INT_TYPE;
}

void SemanticVisitor::visit(FloatLiteralExprNode &node) {
  typeCheckerTable[&node] = SemanticType::FLOAT_TYPE;
}

void SemanticVisitor::visit(ColourLiteralExprNode &node) {
  typeCheckerTable[&node] = SemanticType::COLOUR_TYPE;
}

void SemanticVisitor::visit(PadWidthExprNode &node) {}

void SemanticVisitor::visit(PadHeightExprNode &node) {}

void SemanticVisitor::visit(ReadExprNode &node) {}

void SemanticVisitor::visit(RandiExprNode &node) {}

void SemanticVisitor::visit(AssignmentStmt &node) {}

void SemanticVisitor::visit(VariableDeclStmt &node) {}

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
