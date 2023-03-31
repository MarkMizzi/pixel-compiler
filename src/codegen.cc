#include "codegen.hh"
#include "ast.hh"

#include <sstream>

namespace codegen {

void CodeGenerator::visit(ast::BinaryExprNode &node) {
  rvisitChildren(&node);
  switch (node.op) {
  case ast::BinaryExprNode::BinaryOp::ADD:
    addInstr({PixIROpcode::ADD});
    break;
  case ast::BinaryExprNode::BinaryOp::SUB:
    addInstr({PixIROpcode::SUB});
    break;
  case ast::BinaryExprNode::BinaryOp::DIV:
    addInstr({PixIROpcode::DIV});
    break;
  case ast::BinaryExprNode::BinaryOp::MUL:
    addInstr({PixIROpcode::MUL});
    break;
  case ast::BinaryExprNode::BinaryOp::AND:
    addInstr({PixIROpcode::MIN});
    break;
  case ast::BinaryExprNode::BinaryOp::OR:
    addInstr({PixIROpcode::MAX});
    break;
  case ast::BinaryExprNode::BinaryOp::GREATER:
    addInstr({PixIROpcode::GT});
    break;
  case ast::BinaryExprNode::BinaryOp::LESS:
    addInstr({PixIROpcode::LT});
    break;
  case ast::BinaryExprNode::BinaryOp::EQ:
    addInstr({PixIROpcode::EQ});
    break;
  case ast::BinaryExprNode::BinaryOp::NEQ:
    addInstr({PixIROpcode::EQ});
    addInstr({PixIROpcode::PUSH, "1"});
    addInstr({PixIROpcode::SUB});
    break;
  case ast::BinaryExprNode::BinaryOp::GE:
    addInstr({PixIROpcode::GE});
    break;
  case ast::BinaryExprNode::BinaryOp::LE:
    addInstr({PixIROpcode::LE});
    break;
  }
}

void CodeGenerator::visit(ast::UnaryExprNode &node) {
  rvisitChildren(&node);
  switch (node.op) {
  case ast::UnaryExprNode::UnaryOp::NOT:
    addInstr({PixIROpcode::PUSH, "1"});
    addInstr({PixIROpcode::SUB});
    break;
  case ast::UnaryExprNode::UnaryOp::MINUS:
    addInstr({PixIROpcode::PUSH, "0"});
    addInstr({PixIROpcode::SUB});
    break;
  }
}

void CodeGenerator::visit(ast::FunctionCallNode &node) {
  rvisitChildren(&node);
  addInstr({PixIROpcode::PUSH, std::to_string(node.children().size())});
  addInstr({PixIROpcode::PUSH, "." + node.funcName});
}

void CodeGenerator::visit(ast::IdExprNode &node) {
  // NOTE: unsafe unwrapping here because SemanticVisitor has already dealth
  // with error case.
  auto [depth, index] = frameIndexMap->getDepthAndIndex(node.id);

  addInstr({PixIROpcode::PUSH, "[" + std::to_string(frameNumber - depth) + ":" +
                                   std::to_string(index) + "]"});
}

void CodeGenerator::visit(ast::BoolLiteralExprNode &node) {
  if (node.x) {
    addInstr({PixIROpcode::PUSH, "1"});
  } else {
    addInstr({PixIROpcode::PUSH, "0"});
  }
}

void CodeGenerator::visit(ast::IntLiteralExprNode &node) {
  addInstr({PixIROpcode::PUSH, std::to_string(node.x)});
}

void CodeGenerator::visit(ast::FloatLiteralExprNode &node) {
  addInstr({PixIROpcode::PUSH, std::to_string(node.x)});
}

void CodeGenerator::visit(ast::ColourLiteralExprNode &node) {
  std::stringstream ss;
  ss << "#" << std::hex << node.colour;
  addInstr({PixIROpcode::PUSH, ss.str()});
}

void CodeGenerator::visit(ast::PadWidthExprNode &) {
  addInstr({PixIROpcode::WIDTH});
}

void CodeGenerator::visit(ast::PadHeightExprNode &) {
  addInstr({PixIROpcode::HEIGHT});
}

void CodeGenerator::visit(ast::ReadExprNode &node) {
  rvisitChildren(&node);
  addInstr({PixIROpcode::READ});
}

void CodeGenerator::visit(ast::RandiExprNode &node) {
  rvisitChildren(&node);
  addInstr({PixIROpcode::IRND});
}

void CodeGenerator::visit(ast::AssignmentStmt &node) {
  // NOTE: unsafe unwrapping here because SemanticVisitor has already dealth
  // with error case.
  auto [depth, index] = frameIndexMap->getDepthAndIndex(node.id);

  addInstr({PixIROpcode::PUSH, std::to_string(frameNumber - depth)});
  addInstr({PixIROpcode::PUSH, std::to_string(index)});
  visitChildren(&node);
  addInstr({PixIROpcode::ST});
}

// nothing to generate here. Space for variables is allocated when entering a
// BlockStmt.
void CodeGenerator::visit(ast::VariableDeclStmt &) {}

void CodeGenerator::visit(ast::PrintStmt &node) {
  rvisitChildren(&node);
  addInstr({PixIROpcode::PRINT});
}

void CodeGenerator::visit(ast::DelayStmt &node) {
  rvisitChildren(&node);
  addInstr({PixIROpcode::DELAY});
}

void CodeGenerator::visit(ast::PixelStmt &node) {
  rvisitChildren(&node);
  addInstr({PixIROpcode::PIXEL});
}

void CodeGenerator::visit(ast::PixelRStmt &node) {
  rvisitChildren(&node);
  addInstr({PixIROpcode::PIXELR});
}

void CodeGenerator::visit(ast::ReturnStmt &node) {
  rvisitChildren(&node);
  addInstr({PixIROpcode::RET});
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

  head->instrs.push_back({PixIROpcode::PUSH, ifBlock});
  head->instrs.push_back({PixIROpcode::CJMP2});

  elseBlock->instrs.push_back({PixIROpcode::PUSH, after});
  elseBlock->instrs.push_back({PixIROpcode::JMP});

  ifBlock->instrs.push_back({PixIROpcode::PUSH, after});
  ifBlock->instrs.push_back({PixIROpcode::JMP});
}

void CodeGenerator::visit(ast::ForStmt &node) {
  BasicBlock *head, *after;

  beginBlock();
  node.varDecl->accept(this);
  endBlock();

  beginBlock();
  node.cond->accept(this);
  // !node.cond.
  addInstr({PixIROpcode::PUSH, "1"});
  addInstr({PixIROpcode::SUB});
  head = blockStack.top();
  endBlock();

  beginBlock();
  node.body->accept(this);
  node.assignment->accept(this);
  addInstr({PixIROpcode::PUSH, head});
  addInstr({PixIROpcode::JMP});
  endBlock();

  beginBlock();
  after = blockStack.top();
  endBlock();

  head->instrs.push_back({PixIROpcode::PUSH, after});
  head->instrs.push_back({PixIROpcode::CJMP2});
}

void CodeGenerator::visit(ast::WhileStmt &node) {
  BasicBlock *head, *after;

  beginBlock();
  node.cond->accept(this);
  // !node.cond.
  addInstr({PixIROpcode::PUSH, "1"});
  addInstr({PixIROpcode::SUB});
  head = blockStack.top();
  endBlock();

  beginBlock();
  node.body->accept(this);
  addInstr({PixIROpcode::PUSH, head});
  addInstr({PixIROpcode::JMP});
  endBlock();

  beginBlock();
  after = blockStack.top();
  endBlock();

  head->instrs.push_back({PixIROpcode::PUSH, after});
  head->instrs.push_back({PixIROpcode::CJMP2});
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

void CodeGenerator::linearizeCode() {
  for (PixIRFunction &func : pixIRCode) {
    int offset = 0;
    std::map<BasicBlock *, int> offsets;

    // compute local offsets for each block
    for (BasicBlock &block : func.blocks) {
      offsets.insert({&block, offset});
      offset += block.instrs.size();
    }

    // use local offsets to convert BasicBlock * references in push instructions
    // to PC offsets.
    for (BasicBlock &block : func.blocks) {
      for (int i = 0; i < block.instrs.size(); i++) {
        PixIRInstruction &instr = block.instrs[i];
        if (instr.opcode == PixIROpcode::PUSH &&
            std::holds_alternative<BasicBlock *>(instr.data)) {
          int pc_offset =
              offsets[&block] - offsets[std::get<BasicBlock *>(instr.data)];
          instr.data = std::string("#PC") + (pc_offset >= 0 ? "+" : "") +
                       std::to_string(pc_offset);
        }
      }
    }

    // remove empty blocks in one pass. This works because an empty block has
    // the next offset as the next block.
    for (auto it = func.blocks.begin(); it != func.blocks.end(); ++it) {
      if (it->instrs.size() == 0) {
        --it;
        func.blocks.erase(it + 1);
      }
    }
  }
}

std::string to_string(const PixIROpcode type) {
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
