#include "semantic_visitor.hh"
#include "ast.hh"

#include <stdexcept>

namespace ast {

#define CHECK_TYPE(NODEPTR, TYPE)                                              \
  if (typeCheckerTable().at(NODEPTR) != (TYPE)) {                              \
    throw SemanticError(std::string("Expected type ") + (TYPE).to_string() +   \
                            ", found incompatible type " +                     \
                            typeCheckerTable().at(NODEPTR).to_string() + ".",  \
                        (NODEPTR)->loc);                                       \
  }

void SemanticVisitor::visit(BinaryExprNode &node) {
  visitChildren(&node);

  const SemanticType &leftType = typeCheckerTable().at(node.left.get()),
                     &rightType = typeCheckerTable().at(node.right.get());

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
    typeCheckerTable().insert({&node, SemanticType(leftType)});
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
    typeCheckerTable().insert({&node, Typename::FLOAT});
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
    typeCheckerTable().insert({&node, Typename::BOOL});
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
    typeCheckerTable().insert({&node, Typename::BOOL});
    break;

  case BinaryExprNode::BinaryOp::EQ:
  case BinaryExprNode::BinaryOp::NEQ:
    if (leftType != rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    typeCheckerTable().insert({&node, Typename::BOOL});
    break;
  }
}

void SemanticVisitor::visit(UnaryExprNode &node) {
  visitChildren(&node);

  const SemanticType &operandType = typeCheckerTable().at(&node);

  switch (node.op) {
  case UnaryExprNode::UnaryOp::MINUS:
    if (operandType != Typename::INT && operandType != Typename::FLOAT) {
      throw SemanticError(
          "Expected operand to unary operator to have a numeric type",
          node.loc);
    }
    typeCheckerTable().insert({&node, SemanticType(operandType)});
    break;

  case UnaryExprNode::UnaryOp::NOT:
    if (operandType != Typename::BOOL) {
      throw SemanticError(
          "Expected operand to unary operator to have a boolean type",
          node.loc);
    }
    typeCheckerTable().insert({&node, Typename::BOOL});
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

  for (size_t i = 0; i < argTypes.size(); i++) {
    const SemanticType &type = typeCheckerTable().at(node.args[i].get());
    if (SemanticType(argTypes[i]) != type) {
      throw SemanticError(
          std::to_string(i) + "th argument has wrong type, expected " +
              ast::to_string(argTypes[i]) + ", got " + type.to_string(),
          node.loc);
    }
  }

  typeCheckerTable().insert({&node, SemanticType(retType)});
}

void SemanticVisitor::visit(IdExprNode &node) {
  std::optional<SymbolTableEntry> entry = currentScope->get(node.id);
  if (!entry.has_value()) {
    throw SemanticError("Symbol " + node.id + " is not in scope.", node.loc);
  }

  typeCheckerTable().insert({&node, entry.value().type});
}

void SemanticVisitor::visit(BoolLiteralExprNode &node) {
  typeCheckerTable().insert({&node, SemanticType(Typename::BOOL)});
}

void SemanticVisitor::visit(IntLiteralExprNode &node) {
  typeCheckerTable().insert({&node, SemanticType(Typename::INT)});
}

void SemanticVisitor::visit(FloatLiteralExprNode &node) {
  typeCheckerTable().insert({&node, SemanticType(Typename::FLOAT)});
}

void SemanticVisitor::visit(ColourLiteralExprNode &node) {
  typeCheckerTable().insert({&node, SemanticType(Typename::COLOUR)});
}

void SemanticVisitor::visit(PadWidthExprNode &node) {
  typeCheckerTable().insert({&node, SemanticType(Typename::INT)});
}

void SemanticVisitor::visit(PadHeightExprNode &node) {
  typeCheckerTable().insert({&node, SemanticType(Typename::INT)});
}

void SemanticVisitor::visit(ReadExprNode &node) {
  visitChildren(&node);

  CHECK_TYPE(node.x.get(), SemanticType(Typename::INT));
  CHECK_TYPE(node.y.get(), SemanticType(Typename::INT));

  typeCheckerTable().insert({&node, SemanticType(Typename::COLOUR)});
}

void SemanticVisitor::visit(RandiExprNode &node) {
  visitChildren(&node);

  CHECK_TYPE(node.operand.get(), SemanticType(Typename::INT));

  typeCheckerTable().insert({&node, SemanticType(Typename::INT)});
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

  std::optional<SymbolTableEntry> entry = currentScope->get(node.id);
  if (entry.has_value()) {
    throw SemanticError("Symbol " + node.id + " defined twice in scope.",
                        node.loc);
  }

  SemanticType type = SemanticType(node.type);
  CHECK_TYPE(node.initExpr.get(), type);

  currentScope->add(node.id, SymbolTableEntry{type});
}

void SemanticVisitor::visit(PrintStmt &node) { visitChildren(&node); }

void SemanticVisitor::visit(DelayStmt &node) {
  visitChildren(&node);

  CHECK_TYPE(node.expr.get(), SemanticType(Typename::INT));
}

void SemanticVisitor::visit(PixelStmt &node) {
  visitChildren(&node);

  CHECK_TYPE(node.x.get(), SemanticType(Typename::INT));
  CHECK_TYPE(node.y.get(), SemanticType(Typename::INT));
  CHECK_TYPE(node.colour.get(), SemanticType(Typename::COLOUR));
}

void SemanticVisitor::visit(PixelRStmt &node) {
  visitChildren(&node);

  CHECK_TYPE(node.x.get(), SemanticType(Typename::INT));
  CHECK_TYPE(node.y.get(), SemanticType(Typename::INT));
  CHECK_TYPE(node.w.get(), SemanticType(Typename::INT));
  CHECK_TYPE(node.h.get(), SemanticType(Typename::INT));
  CHECK_TYPE(node.colour.get(), SemanticType(Typename::COLOUR));
}

void SemanticVisitor::visit(ReturnStmt &node) {
  visitChildren(&node);

  std::optional<SemanticFunctionType> funcType = currentScope->getFuncType();

  if (!funcType.has_value()) {
    throw SemanticError("Return statement outside of function body.", node.loc);
  }

  const SemanticType &retType = funcType.value().first;
  const SemanticType &type = typeCheckerTable().at(node.expr.get());

  if (retType != type) {
    throw SemanticError("Return type does not match expected type, expected " +
                            retType.to_string() + ", got " + type.to_string(),
                        node.loc);
  }
}

void SemanticVisitor::visit(IfElseStmt &node) {
  visitChildren(&node);

  CHECK_TYPE(node.cond.get(), SemanticType(Typename::BOOL));
}

void SemanticVisitor::visit(ForStmt &node) {
  enterScope(&node);
  visitChildren(&node);
  CHECK_TYPE(node.cond.get(), SemanticType(Typename::BOOL));
  exitScope();
}

void SemanticVisitor::visit(WhileStmt &node) {
  visitChildren(&node);

  CHECK_TYPE(node.cond.get(), SemanticType(Typename::BOOL));
}

void SemanticVisitor::visit(FuncDeclStmt &node) {
  SemanticFunctionType funcType{node.retType,
                                std::vector<Typename>(node.params.size())};
  std::transform(node.params.begin(), node.params.end(),
                 funcType.second.begin(),
                 [](const FormalParam &param) { return param.second; });
  currentScope->add(node.funcName, SymbolTableEntry{SemanticType(funcType)});

  enterScope(&node, funcType);
  for (auto const &[paramName, paramType] : node.params) {
    currentScope->add(paramName, SymbolTableEntry{SemanticType(paramType)});
  }
  // this will create a new scope for the block, but that's ok. The formal
  // params will be available in the new scope.
  visitChildren(&node);
  exitScope();
}

void SemanticVisitor::visit(BlockStmt &node) {
  enterScope(&node);
  visitChildren(&node);
  exitScope();
}

void SemanticVisitor::visit(TranslationUnit &node) {
  enterScope(&node);
  visitChildren(&node);
  exitScope();
}

} // end namespace ast
