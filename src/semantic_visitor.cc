#include "semantic_visitor.hh"
#include "ast.hh"

#include <stdexcept>

namespace ast {

#define CHECK_TYPE(NODEPTR, TYPE)                                              \
  if (typeCheckerTable.at(NODEPTR) != (TYPE)) {                                \
    throw SemanticError(std::string("Expected type ") + (TYPE).to_string() +   \
                            ", found incompatible type " +                     \
                            typeCheckerTable.at(NODEPTR).to_string() + ".",    \
                        (NODEPTR)->loc);                                       \
  }

void SemanticVisitor::visit(BinaryExprNode &node) {
  visitChildren(&node);

  const SemanticType &leftType = typeCheckerTable.at(node.left.get()),
                     &rightType = typeCheckerTable.at(node.right.get());

  switch (node.op) {
  case BinaryExprNode::BinaryOp::ADD:
  case BinaryExprNode::BinaryOp::SUB:
  case BinaryExprNode::BinaryOp::MUL:
    if (leftType != rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    if (leftType != Typename::INT && leftType != Typename::FLOAT) {
      throw SemanticError(
          "Expected operands to binary operator to have a numeric type",
          node.loc);
    }
    typeCheckerTable[&node] = SemanticType(leftType);
    break;

  case BinaryExprNode::BinaryOp::DIV:
    if (leftType != rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    if (leftType != Typename::INT && leftType != Typename::FLOAT) {
      throw SemanticError(
          "Expected operands to binary operator to have a numeric type",
          node.loc);
    }
    typeCheckerTable[&node] = Typename::FLOAT;
    break;

  case BinaryExprNode::BinaryOp::AND:
  case BinaryExprNode::BinaryOp::OR:
    if (leftType != rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    if (leftType != Typename::BOOL) {
      throw SemanticError(
          "Expected operands to binary operator to have a boolean type",
          node.loc);
    }
    typeCheckerTable[&node] = Typename::BOOL;
    break;

  case BinaryExprNode::BinaryOp::GREATER:
  case BinaryExprNode::BinaryOp::LESS:
  case BinaryExprNode::BinaryOp::GE:
  case BinaryExprNode::BinaryOp::LE:
    if (leftType != rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    if (leftType != Typename::INT && leftType != Typename::FLOAT) {
      throw SemanticError(
          "Expected operands to binary operator to have a numeric type",
          node.loc);
    }
    typeCheckerTable[&node] = Typename::BOOL;
    break;

  case BinaryExprNode::BinaryOp::EQ:
  case BinaryExprNode::BinaryOp::NEQ:
    if (leftType != rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    typeCheckerTable[&node] = Typename::BOOL;
    break;
  }
}

void SemanticVisitor::visit(UnaryExprNode &node) {
  visitChildren(&node);

  const SemanticType &operandType = typeCheckerTable.at(&node);

  switch (node.op) {
  case UnaryExprNode::UnaryOp::MINUS:
    if (operandType != Typename::INT && operandType != Typename::FLOAT) {
      throw SemanticError(
          "Expected operand to unary operator to have a numeric type",
          node.loc);
    }
    typeCheckerTable[&node] = SemanticType(operandType);
    break;

  case UnaryExprNode::UnaryOp::NOT:
    if (operandType != Typename::BOOL) {
      throw SemanticError(
          "Expected operand to unary operator to have a boolean type",
          node.loc);
    }
    typeCheckerTable[&node] = Typename::BOOL;
    break;
  }
}

void SemanticVisitor::visit(FunctionCallNode &node) {
  visitChildren(&node);

  std::optional<SymbolTableEntry> entry = currentScope->get(node.funcName);
  if (!entry.has_value()) {
    throw SemanticError("Symbol " + node.funcName + " is not in scope.",
                        node.loc);
  }

  const SemanticFunctionType *type = entry.value().type.getFunctionType();
  if (type == nullptr) {
    throw SemanticError("Symbol " + node.funcName + " is not a function.",
                        node.loc);
  }

  auto const &[retType, argTypes] = *type;
  if (argTypes.size() != node.args.size()) {
    throw SemanticError(
        "Wrong number of arguments passed to function, expected " +
            std::to_string(argTypes.size()) + ", got " +
            std::to_string(node.args.size()),
        node.loc);
  }

  for (int i = 0; i < argTypes.size(); i++) {
    const SemanticType &type = typeCheckerTable.at(node.args[i].get());
    if (SemanticType(argTypes[i]) != type) {
      throw SemanticError(
          std::to_string(i) + "th argument has wrong type, expected " +
              ast::to_string(argTypes[i]) + ", got " + type.to_string(),
          node.loc);
    }
  }

  typeCheckerTable[&node] = SemanticType(retType);
}

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
  visitChildren(&node);

  CHECK_TYPE(node.x.get(), SemanticType(Typename::INT));
  CHECK_TYPE(node.y.get(), SemanticType(Typename::INT));

  typeCheckerTable[&node] = SemanticType(Typename::COLOUR);
}

void SemanticVisitor::visit(RandiExprNode &node) {
  visitChildren(&node);

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
