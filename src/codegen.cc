#include "codegen.hh"
#include "ast.hh"

#include <sstream>
#include <stdexcept>

namespace codegen {

void CodeGenerator::visit(ast::BinaryExprNode &node) {
  rvisitChildren(&node);
  switch (node.op) {
  case ast::BinaryExprNode::BinaryOp::ADD:
    addInstr({PixIRInstructionType::ADD});
    break;
  case ast::BinaryExprNode::BinaryOp::SUB:
    addInstr({PixIRInstructionType::SUB});
    break;
  case ast::BinaryExprNode::BinaryOp::DIV:
    addInstr({PixIRInstructionType::DIV});
    break;
  case ast::BinaryExprNode::BinaryOp::MUL:
    addInstr({PixIRInstructionType::MUL});
    break;
  case ast::BinaryExprNode::BinaryOp::AND:
    addInstr({PixIRInstructionType::MIN});
    break;
  case ast::BinaryExprNode::BinaryOp::OR:
    addInstr({PixIRInstructionType::MAX});
    break;
  case ast::BinaryExprNode::BinaryOp::GREATER:
    addInstr({PixIRInstructionType::GT});
    break;
  case ast::BinaryExprNode::BinaryOp::LESS:
    addInstr({PixIRInstructionType::LT});
    break;
  case ast::BinaryExprNode::BinaryOp::EQ:
    addInstr({PixIRInstructionType::EQ});
    break;
  case ast::BinaryExprNode::BinaryOp::NEQ:
    addInstr({PixIRInstructionType::EQ});
    addInstr({PixIRInstructionType::PUSH, "1"});
    addInstr({PixIRInstructionType::SUB});
    break;
  case ast::BinaryExprNode::BinaryOp::GE:
    addInstr({PixIRInstructionType::GE});
    break;
  case ast::BinaryExprNode::BinaryOp::LE:
    addInstr({PixIRInstructionType::LE});
    break;
  }
}

void CodeGenerator::visit(ast::UnaryExprNode &node) {
  rvisitChildren(&node);
  switch (node.op) {
  case ast::UnaryExprNode::UnaryOp::NOT:
    addInstr({PixIRInstructionType::PUSH, "1"});
    addInstr({PixIRInstructionType::SUB});
    break;
  case ast::UnaryExprNode::UnaryOp::MINUS:
    addInstr({PixIRInstructionType::PUSH, "0"});
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

void CodeGenerator::visit(ast::IdExprNode &node) {
  // NOTE: unsafe unwrapping here because SemanticVisitor has already dealth
  // with error case.
  auto [depth, index] = frameIndexMap->getDepthAndIndex(node.id);

  addInstr(
      {PixIRInstructionType::PUSH, "[" + std::to_string(frameNumber - depth) +
                                       ":" + std::to_string(index) + "]"});
}

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

void CodeGenerator::visit(ast::PadWidthExprNode &) {
  addInstr({PixIRInstructionType::WIDTH});
}

void CodeGenerator::visit(ast::PadHeightExprNode &) {
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

void CodeGenerator::visit(ast::AssignmentStmt &node) {
  // NOTE: unsafe unwrapping here because SemanticVisitor has already dealth
  // with error case.
  auto [depth, index] = frameIndexMap->getDepthAndIndex(node.id);

  addInstr({PixIRInstructionType::PUSH, std::to_string(frameNumber - depth)});
  addInstr({PixIRInstructionType::PUSH, std::to_string(index)});
  visitChildren(&node);
  addInstr({PixIRInstructionType::ST});
}

// nothing to generate here. Space for variables is allocated when entering a
// BlockStmt.
void CodeGenerator::visit(ast::VariableDeclStmt &) {}

void CodeGenerator::visit(ast::PrintStmt &node) {
  rvisitChildren(&node);
  addInstr({PixIRInstructionType::PRINT});
}

void CodeGenerator::visit(ast::DelayStmt &node) {
  rvisitChildren(&node);
  addInstr({PixIRInstructionType::DELAY});
}

void CodeGenerator::visit(ast::PixelStmt &node) {
  rvisitChildren(&node);
  addInstr({PixIRInstructionType::PIXEL});
}

void CodeGenerator::visit(ast::PixelRStmt &node) {
  rvisitChildren(&node);
  addInstr({PixIRInstructionType::PIXELR});
}

void CodeGenerator::visit(ast::ReturnStmt &node) {
  rvisitChildren(&node);
  addInstr({PixIRInstructionType::RET});
}

void CodeGenerator::visit(ast::IfElseStmt &node) {
  BasicBlock *head, *ifBlock, *elseBlock, *after;

  beginBlock();
  node.cond->accept(this);
  head = blockStack.top();
  endBlock();

  beginBlock();
  if (node.elseBody) {
    node.elseBody->accept(this);
  }
  elseBlock = blockStack.top();
  endBlock();

  beginBlock();
  node.ifBody->accept(this);
  ifBlock = blockStack.top();
  endBlock();

  beginBlock();
  after = blockStack.top();
  endBlock();

  head->instrs.push_back({PixIRInstructionType::PUSH, ifBlock});
  head->instrs.push_back({PixIRInstructionType::CJMP2});

  elseBlock->instrs.push_back({PixIRInstructionType::PUSH, after});
  elseBlock->instrs.push_back({PixIRInstructionType::JMP});

  ifBlock->instrs.push_back({PixIRInstructionType::PUSH, after});
  ifBlock->instrs.push_back({PixIRInstructionType::JMP});
}

void CodeGenerator::visit(ast::ForStmt &node) {
  BasicBlock *head, *after;

  beginBlock();
  node.varDecl->accept(this);
  endBlock();

  beginBlock();
  node.cond->accept(this);
  // !node.cond.
  addInstr({PixIRInstructionType::PUSH, "1"});
  addInstr({PixIRInstructionType::SUB});
  head = blockStack.top();
  endBlock();

  beginBlock();
  node.body->accept(this);
  node.assignment->accept(this);
  addInstr({PixIRInstructionType::PUSH, head});
  addInstr({PixIRInstructionType::JMP});
  endBlock();

  beginBlock();
  after = blockStack.top();
  endBlock();

  head->instrs.push_back({PixIRInstructionType::PUSH, after});
  head->instrs.push_back({PixIRInstructionType::CJMP2});
}

void CodeGenerator::visit(ast::WhileStmt &node) {
  BasicBlock *head, *after;

  beginBlock();
  node.cond->accept(this);
  // !node.cond.
  addInstr({PixIRInstructionType::PUSH, "1"});
  addInstr({PixIRInstructionType::SUB});
  head = blockStack.top();
  endBlock();

  beginBlock();
  node.body->accept(this);
  addInstr({PixIRInstructionType::PUSH, head});
  addInstr({PixIRInstructionType::JMP});
  endBlock();

  beginBlock();
  after = blockStack.top();
  endBlock();

  head->instrs.push_back({PixIRInstructionType::PUSH, after});
  head->instrs.push_back({PixIRInstructionType::CJMP2});
}

void CodeGenerator::visit(ast::FuncDeclStmt &node) {
  beginFunc(node.funcName);
  enterFuncDefFrame(node);
  visitChildren(&node);
  exitFuncDefFrame();
  endFunc();
}

void CodeGenerator::visit(ast::BlockStmt &node) {
  enterFrame(&node);
  visitChildren(&node);
  exitFrame();
}

void CodeGenerator::visit(ast::TranslationUnit &node) {
  beginFunc(MAIN_FUNC_NAME);
  enterMainFrame(node);
  visitChildren(&node);
  exitMainFrame();
  endFunc();
}

void CodeGenerator::linearizeCode() {}

std::string to_string(const PixIRInstructionType type) {
  switch (type) {
  case ADD:
    return "ADD";
  case SUB:
    return "SUB";
  case MUL:
    return "MUL";
  case DIV:
    return "DIV";
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
  case HALT:
    return "HALT";
  }
  return ""; // please compiler
}

} // end namespace codegen
