#include "codegen.hh"
#include "ast.hh"

#include <sstream>
#include <stdexcept>

namespace codegen {

void CodeGenerator::visit(ast::BinaryExprNode &node) {
  switch (node.op) {
  case ast::BinaryExprNode::BinaryOp::ADD:
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::ADD});
    break;
  case ast::BinaryExprNode::BinaryOp::SUB:
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::SUB});
    break;
  case ast::BinaryExprNode::BinaryOp::DIV:
    rvisitChildren(&node);
    throw std::logic_error("Not implemented yet.");
    break;
  case ast::BinaryExprNode::BinaryOp::MUL:
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::MUL});
    break;
  case ast::BinaryExprNode::BinaryOp::AND:
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::MIN});
    break;
  case ast::BinaryExprNode::BinaryOp::OR:
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::MAX});
    break;
  case ast::BinaryExprNode::BinaryOp::GREATER:
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::GT});
    break;
  case ast::BinaryExprNode::BinaryOp::LESS:
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::LT});
    break;
  case ast::BinaryExprNode::BinaryOp::EQ:
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::EQ});
    break;
  case ast::BinaryExprNode::BinaryOp::NEQ:
    addInstr({PixIRInstructionType::PUSH, "1"});
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::EQ});
    addInstr({PixIRInstructionType::SUB});
    break;
  case ast::BinaryExprNode::BinaryOp::GE:
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::GE});
    break;
  case ast::BinaryExprNode::BinaryOp::LE:
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::LE});
    break;
  }
}

void CodeGenerator::visit(ast::UnaryExprNode &node) {
  switch (node.op) {
  case ast::UnaryExprNode::UnaryOp::NOT:
    addInstr({PixIRInstructionType::PUSH, "1"});
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::SUB});
    break;
  case ast::UnaryExprNode::UnaryOp::MINUS:
    addInstr({PixIRInstructionType::PUSH, "0"});
    rvisitChildren(&node);
    addInstr({PixIRInstructionType::SUB});
    break;
  }
}

void CodeGenerator::visit(ast::FunctionCallNode &node) {
  rvisitChildren(&node);
  addInstr(
      {PixIRInstructionType::PUSH, std::to_string(node.children().size())});
  addInstr({PixIRInstructionType::PUSH, "." + node.funcName});
}

void CodeGenerator::visit(ast::IdExprNode &node) {}

void CodeGenerator::visit(ast::BoolLiteralExprNode &node) {
  if (node.x) {
    addInstr({PixIRInstructionType::PUSH, "1"});
  } else {
    addInstr({PixIRInstructionType::PUSH, "0"});
  }
}

void CodeGenerator::visit(ast::IntLiteralExprNode &node) {
  addInstr({PixIRInstructionType::PUSH, std::to_string(node.x)});
}

void CodeGenerator::visit(ast::FloatLiteralExprNode &node) {
  addInstr({PixIRInstructionType::PUSH, std::to_string(node.x)});
}

void CodeGenerator::visit(ast::ColourLiteralExprNode &node) {
  std::stringstream ss;
  ss << "#" << std::hex << node.colour;
  addInstr({PixIRInstructionType::PUSH, ss.str()});
}

void CodeGenerator::visit(ast::PadWidthExprNode &node) {
  addInstr({PixIRInstructionType::WIDTH});
}

void CodeGenerator::visit(ast::PadHeightExprNode &node) {
  addInstr({PixIRInstructionType::HEIGHT});
}

void CodeGenerator::visit(ast::ReadExprNode &node) {
  rvisitChildren(&node);
  addInstr({PixIRInstructionType::READ});
}

void CodeGenerator::visit(ast::RandiExprNode &node) {
  rvisitChildren(&node);
  addInstr({PixIRInstructionType::IRND});
}

void CodeGenerator::visit(ast::AssignmentStmt &node) {}

void CodeGenerator::visit(ast::VariableDeclStmt &node) {}

void CodeGenerator::visit(ast::PrintStmt &node) {}

void CodeGenerator::visit(ast::DelayStmt &node) {}

void CodeGenerator::visit(ast::PixelStmt &node) {}

void CodeGenerator::visit(ast::PixelRStmt &node) {}

void CodeGenerator::visit(ast::ReturnStmt &node) {}

void CodeGenerator::visit(ast::IfElseStmt &node) {}

void CodeGenerator::visit(ast::ForStmt &node) {}

void CodeGenerator::visit(ast::WhileStmt &node) {}

void CodeGenerator::visit(ast::FuncDeclStmt &node) {}

void CodeGenerator::visit(ast::BlockStmt &node) {}

void CodeGenerator::visit(ast::TranslationUnit &node) {}

std::string to_string(const PixIRInstructionType type) {
  switch (type) {
  case ADD:
    return "ADD";
  case SUB:
    return "SUB";
  case MUL:
    return "MUL";
  case INC:
    return "INC";
  case DEC:
    return "DEC";
  case MAX:
    return "MAX";
  case MIN:
    return "MIN";
  case IRND:
    return "IRND";
  case LT:
    return "LT";
  case LE:
    return "LE";
  case EQ:
    return "EQ";
  case GT:
    return "GT";
  case GE:
    return "GE";
  case PUSH:
    return "PUSH";
  case JMP:
    return "JMP";
  case CJMP:
    return "CJMP";
  case CJMP2:
    return "CJMP2";
  case CALL:
    return "CALL";
  case RET:
    return "RET";
  case ST:
    return "ST";
  case ALLOC:
    return "ALLOC";
  case OFRAME:
    return "OFRAME";
  case CFRAME:
    return "CFRAME";
  case DELAY:
    return "DELAY";
  case PIXEL:
    return "PIXEL";
  case PIXELR:
    return "PIXELR";
  case CLEAR:
    return "CLEAR";
  case READ:
    return "READ";
  case WIDTH:
    return "WIDTH";
  case HEIGHT:
    return "HEIGHT";
  case PRINT:
    return "PRINT";
  case DUP:
    return "DUP";
  }
  return ""; // please compiler
}

} // end namespace codegen
