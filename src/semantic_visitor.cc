#include "semantic_visitor.hh"
#include "ast.hh"

#include <memory>
#include <stdexcept>

namespace ast {

#define CHECK_TYPE(NODEPTR, TYPE)                                              \
  if (*typeCheckerTable().at(NODEPTR) != (TYPE)) {                             \
    throw SemanticError(std::string("Expected type ") + (TYPE).to_string() +   \
                            ", found incompatible type " +                     \
                            typeCheckerTable().at(NODEPTR)->to_string() + ".", \
                        (NODEPTR)->loc);                                       \
  }

void SemanticVisitor::visit(BinaryExprNode &node) {
  visitChildren(&node);

  const ast::TypeNodePtr &leftType = typeCheckerTable().at(node.left.get()),
                         &rightType = typeCheckerTable().at(node.right.get());

  switch (node.op) {
  case BinaryExprNode::BinaryOp::ADD:
  case BinaryExprNode::BinaryOp::SUB:
  case BinaryExprNode::BinaryOp::MUL:
    if (*leftType != *rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    if (*leftType != IntTypeNode() && *leftType != FloatTypeNode()) {
      throw SemanticError(
          "Expected operands to binary operator to have a numeric type",
          node.loc);
    }
    typeCheckerTable().insert({&node, leftType->copy()});
    break;

  case BinaryExprNode::BinaryOp::DIV:
    if (*leftType != *rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    if (*leftType != IntTypeNode() && *leftType != FloatTypeNode()) {
      throw SemanticError(
          "Expected operands to binary operator to have a numeric type",
          node.loc);
    }
    typeCheckerTable().insert({&node, std::make_unique<FloatTypeNode>()});
    break;

  case BinaryExprNode::BinaryOp::AND:
  case BinaryExprNode::BinaryOp::OR:
    if (*leftType != *rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    if (*leftType != BoolTypeNode()) {
      throw SemanticError(
          "Expected operands to binary operator to have a boolean type",
          node.loc);
    }
    typeCheckerTable().insert({&node, std::make_unique<BoolTypeNode>()});
    break;

  case BinaryExprNode::BinaryOp::GREATER:
  case BinaryExprNode::BinaryOp::LESS:
  case BinaryExprNode::BinaryOp::GE:
  case BinaryExprNode::BinaryOp::LE:
    if (*leftType != *rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    if (*leftType != IntTypeNode() && *leftType != FloatTypeNode()) {
      throw SemanticError(
          "Expected operands to binary operator to have a numeric type",
          node.loc);
    }
    typeCheckerTable().insert({&node, std::make_unique<BoolTypeNode>()});
    break;

  case BinaryExprNode::BinaryOp::EQ:
  case BinaryExprNode::BinaryOp::NEQ:
    if (*leftType != *rightType) {
      throw SemanticError(
          "Expected operands to binary operator to be of the same type",
          node.loc);
    }
    typeCheckerTable().insert({&node, std::make_unique<BoolTypeNode>()});
    break;
  }
}

void SemanticVisitor::visit(UnaryExprNode &node) {
  visitChildren(&node);

  const ast::TypeNodePtr &operandType = typeCheckerTable().at(&node);

  switch (node.op) {
  case UnaryExprNode::UnaryOp::MINUS:
    if (*operandType != IntTypeNode() && *operandType != FloatTypeNode()) {
      throw SemanticError(
          "Expected operand to unary operator to have a numeric type",
          node.loc);
    }
    typeCheckerTable().insert({&node, operandType->copy()});
    break;

  case UnaryExprNode::UnaryOp::NOT:
    if (*operandType != BoolTypeNode()) {
      throw SemanticError(
          "Expected operand to unary operator to have a boolean type",
          node.loc);
    }
    typeCheckerTable().insert({&node, std::make_unique<BoolTypeNode>()});
    break;
  }
}

void SemanticVisitor::visit(FunctionCallNode &node) {
  visitChildren(&node);

  const SymbolTableEntry *entry = currentScope->get(node.funcName);
  if (entry == nullptr) {
    throw SemanticError("Symbol " + node.funcName + " is not in scope.",
                        node.loc);
  }

  if (!entry->type->isFuncType()) {
    throw SemanticError("Symbol " + node.funcName + " is not a function.",
                        node.loc);
  }

  const FunctionTypeNode *funcType =
      dynamic_cast<const ast::FunctionTypeNode *>(entry->type.get());

  if (funcType->argTypes.size() != node.args.size()) {
    throw SemanticError(
        "Wrong number of arguments passed to function, expected " +
            std::to_string(funcType->argTypes.size()) + ", got " +
            std::to_string(node.args.size()),
        node.loc);
  }

  for (size_t i = 0; i < funcType->argTypes.size(); i++) {
    const TypeNodePtr &type = typeCheckerTable().at(node.args[i].get());
    if (*funcType->argTypes[i] != *type) {
      throw SemanticError(
          std::to_string(i) + "th argument has wrong type, expected " +
              funcType->argTypes[i]->to_string() + ", got " + type->to_string(),
          node.loc);
    }
  }

  typeCheckerTable().insert({&node, funcType->retType->copy()});
}

void SemanticVisitor::visit(IdExprNode &node) {
  const SymbolTableEntry *entry = currentScope->get(node.id);
  if (entry == nullptr) {
    throw SemanticError("Symbol " + node.id + " is not in scope.", node.loc);
  }

  typeCheckerTable().insert({&node, entry->type->copy()});
}

void SemanticVisitor::visit(BoolLiteralExprNode &node) {
  typeCheckerTable().insert({&node, std::make_unique<BoolTypeNode>()});
}

void SemanticVisitor::visit(IntLiteralExprNode &node) {
  typeCheckerTable().insert({&node, std::make_unique<IntTypeNode>()});
}

void SemanticVisitor::visit(FloatLiteralExprNode &node) {
  typeCheckerTable().insert({&node, std::make_unique<FloatTypeNode>()});
}

void SemanticVisitor::visit(ColourLiteralExprNode &node) {
  typeCheckerTable().insert({&node, std::make_unique<ColourTypeNode>()});
}

void SemanticVisitor::visit(PadWidthExprNode &node) {
  typeCheckerTable().insert({&node, std::make_unique<IntTypeNode>()});
}

void SemanticVisitor::visit(PadHeightExprNode &node) {
  typeCheckerTable().insert({&node, std::make_unique<IntTypeNode>()});
}

void SemanticVisitor::visit(ReadExprNode &node) {
  visitChildren(&node);

  CHECK_TYPE(node.x.get(), IntTypeNode());
  CHECK_TYPE(node.y.get(), IntTypeNode());

  typeCheckerTable().insert({&node, std::make_unique<ColourTypeNode>()});
}

void SemanticVisitor::visit(RandiExprNode &node) {
  visitChildren(&node);

  CHECK_TYPE(node.operand.get(), IntTypeNode());

  typeCheckerTable().insert({&node, std::make_unique<IntTypeNode>()});
}

void SemanticVisitor::visit(NewArrExprNode &node) {
  visitChildren(&node);

  typeCheckerTable().insert({&node, std::make_unique<ArrayTypeNode>(
                                        node.ofType->copy(), Location{})});
}

void SemanticVisitor::visit(NullArrExprNode &node) {
  visitChildren(&node);

  typeCheckerTable().insert({&node, std::make_unique<ArrayTypeNode>(
                                        node.ofType->copy(), Location{})});
}

void SemanticVisitor::visit(ArrayAccessNode &node) {
  visitChildren(&node);

  const TypeNodePtr &arrType = typeCheckerTable().at(node.array.get());

  if (!arrType->isArrType()) {
    throw SemanticError("Cannot index into expression of non-array type " +
                            arrType->to_string(),
                        node.loc);
  }
  CHECK_TYPE(node.idx.get(), IntTypeNode());

  typeCheckerTable().insert(
      {&node, dynamic_cast<ArrayTypeNode *>(arrType.get())->contained->copy()});
}

void SemanticVisitor::visit(AssignmentStmt &node) {
  visitChildren(&node);

  const ast::TypeNodePtr &leftType = typeCheckerTable().at(node.lvalue.get()),
                         &rightType = typeCheckerTable().at(node.expr.get());

  if (*leftType != *rightType) {
    throw SemanticError("Cannot assign value of type " +
                            rightType->to_string() + " to lvalue of type " +
                            leftType->to_string(),
                        node.loc);
  }
}

void SemanticVisitor::visit(VariableDeclStmt &node) {
  visitChildren(&node);

  const SymbolTableEntry *entry = currentScope->get(node.id);
  if (entry != nullptr) {
    throw SemanticError("Symbol " + node.id + " defined twice in scope.",
                        node.loc);
  }

  TypeNodePtr varType = node.type->copy();
  CHECK_TYPE(node.initExpr.get(), *varType);

  currentScope->add(node.id,
                    std::make_unique<SymbolTableEntry>(std::move(varType)));
}

void SemanticVisitor::visit(PrintStmt &node) { visitChildren(&node); }

void SemanticVisitor::visit(DelayStmt &node) {
  visitChildren(&node);

  CHECK_TYPE(node.expr.get(), IntTypeNode());
}

void SemanticVisitor::visit(PixelStmt &node) {
  visitChildren(&node);

  CHECK_TYPE(node.x.get(), IntTypeNode());
  CHECK_TYPE(node.y.get(), IntTypeNode());
  CHECK_TYPE(node.colour.get(), ColourTypeNode());
}

void SemanticVisitor::visit(PixelRStmt &node) {
  visitChildren(&node);

  CHECK_TYPE(node.x.get(), IntTypeNode());
  CHECK_TYPE(node.y.get(), IntTypeNode());
  CHECK_TYPE(node.w.get(), IntTypeNode());
  CHECK_TYPE(node.h.get(), IntTypeNode());
  CHECK_TYPE(node.colour.get(), ColourTypeNode());
}

void SemanticVisitor::visit(ReturnStmt &node) {
  visitChildren(&node);

  const std::optional<ast::FunctionTypeNode> &funcType =
      currentScope->getFuncType();

  if (!funcType.has_value()) {
    throw SemanticError("Return statement outside of function body.", node.loc);
  }

  const TypeNodePtr &retType = funcType.value().retType;
  const TypeNodePtr &type = typeCheckerTable().at(node.expr.get());

  if (*retType != *type) {
    throw SemanticError("Return type does not match expected type, expected " +
                            retType->to_string() + ", got " + type->to_string(),
                        node.loc);
  }
}

void SemanticVisitor::visit(IfElseStmt &node) {
  visitChildren(&node);

  CHECK_TYPE(node.cond.get(), BoolTypeNode());
}

void SemanticVisitor::visit(ForStmt &node) {
  enterScope(&node);
  visitChildren(&node);
  CHECK_TYPE(node.cond.get(), BoolTypeNode());
  exitScope();
}

void SemanticVisitor::visit(WhileStmt &node) {
  visitChildren(&node);

  CHECK_TYPE(node.cond.get(), BoolTypeNode());
}

void SemanticVisitor::visit(FuncDeclStmt &node) {
  std::vector<TypeNodePtr> argTypes(node.params.size());
  std::transform(
      node.params.begin(), node.params.end(), argTypes.begin(),
      [](const ast::FormalParam &param) { return param.second->copy(); });

  std::unique_ptr<FunctionTypeNode> funcType =
      std::make_unique<FunctionTypeNode>(node.retType->copy(),
                                         std::move(argTypes), Location{});

  currentScope->add(node.funcName,
                    std::make_unique<SymbolTableEntry>(funcType->copy()));

  enterScope(&node, std::move(*funcType));
  for (auto const &[paramName, paramType] : node.params) {
    currentScope->add(paramName,
                      std::make_unique<SymbolTableEntry>(paramType->copy()));
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
