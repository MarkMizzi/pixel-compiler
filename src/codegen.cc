#include "codegen.hh"
#include "ast.hh"

#include <iomanip>
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
  addInstr({PixIROpcode::CALL});
}

void CodeGenerator::visit(ast::IdExprNode &node) {
  // NOTE: unsafe unwrapping here because SemanticVisitor has already dealth
  // with error case.
  auto [depth, index] = frameIndexMap->getDepthAndIndex(node.id);

  addInstr({PixIROpcode::PUSH,
            "[" + std::to_string(index) + ":" + std::to_string(depth) + "]"});
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
  ss << "#" << std::hex << std::setw(6) << std::setfill('0') << node.colour;
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

  rvisitChildren(&node);
  addInstr({PixIROpcode::PUSH, std::to_string(index)});
  addInstr({PixIROpcode::PUSH, std::to_string(depth)});
  addInstr({PixIROpcode::ST});
}

// nothing to generate here. Space for variables is allocated when entering a
// BlockStmt.
void CodeGenerator::visit(ast::VariableDeclStmt &node) {
  // NOTE: unsafe unwrapping here because SemanticVisitor has already dealth
  // with error case.
  auto [depth, index] = frameIndexMap->getDepthAndIndex(node.id);

  rvisitChildren(&node);
  addInstr({PixIROpcode::PUSH, std::to_string(index)});
  addInstr({PixIROpcode::PUSH, std::to_string(depth)});
  addInstr({PixIROpcode::ST});
}

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

void linearizeCode(PixIRCode &pixIRCode) {
  for (std::unique_ptr<PixIRFunction> &func : pixIRCode) {
    int offset = 0;
    std::map<BasicBlock *, int> offsets;

    // compute local offsets for each block
    for (std::unique_ptr<BasicBlock> &block : func->blocks) {
      offsets.insert({block.get(), offset});
      offset += block->instrs.size();
    }

    // use local offsets to convert BasicBlock * references in push instructions
    // to PC offsets.
    for (std::unique_ptr<BasicBlock> &block : func->blocks) {
      for (int i = 0; i < block->instrs.size(); i++) {
        PixIRInstruction &instr = block->instrs[i];
        if (instr.opcode == PixIROpcode::PUSH &&
            std::holds_alternative<BasicBlock *>(instr.data)) {
          int pc_offset = offsets[std::get<BasicBlock *>(instr.data)] -
                          offsets[block.get()];
          instr.data = std::string("#PC") + (pc_offset >= 0 ? "+" : "") +
                       std::to_string(pc_offset);
        }
      }
    }

    // remove empty blocks in one pass. This works because an empty block has
    // the next offset as the next block.
    for (auto it = func->blocks.begin(); it != func->blocks.end(); ++it) {
      if ((*it)->instrs.size() == 0) {
        --it;
        func->blocks.erase(it + 1);
      }
    }
  }
}

void dumpCode(PixIRCode &pixIRCode, std::ostream &s) {
  for (const std::unique_ptr<codegen::PixIRFunction> &func : pixIRCode) {
    s << func->funcName << std::endl;
    for (const std::unique_ptr<codegen::BasicBlock> &block : func->blocks) {
      for (const codegen::PixIRInstruction &instr : block->instrs) {
        s << "\t" << instr.to_string() << std::endl;
      }
    }
  }
}

std::string to_string(const PixIROpcode type) {
  switch (type) {
  case ADD:
    return "add";
  case SUB:
    return "sub";
  case MUL:
    return "mul";
  case DIV:
    return "div";
  case INC:
    return "inc";
  case DEC:
    return "dec";
  case MAX:
    return "max";
  case MIN:
    return "min";
  case IRND:
    return "irnd";
  case LT:
    return "lt";
  case LE:
    return "le";
  case EQ:
    return "eq";
  case GT:
    return "gt";
  case GE:
    return "ge";
  case PUSH:
    return "push";
  case JMP:
    return "jmp";
  case CJMP:
    return "cjmp";
  case CJMP2:
    return "cjmp2";
  case CALL:
    return "call";
  case RET:
    return "ret";
  case ST:
    return "st";
  case ALLOC:
    return "alloc";
  case OFRAME:
    return "oframe";
  case CFRAME:
    return "cframe";
  case DELAY:
    return "delay";
  case PIXEL:
    return "pixel";
  case PIXELR:
    return "pixelr";
  case CLEAR:
    return "clear";
  case READ:
    return "read";
  case WIDTH:
    return "width";
  case HEIGHT:
    return "height";
  case PRINT:
    return "print";
  case DUP:
    return "dup";
  case HALT:
    return "halt";
  }
  return ""; // please compiler
}

} // end namespace codegen
