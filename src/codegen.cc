#include "codegen.hh"
#include "ast.hh"

#include <iomanip>
#include <sstream>

namespace codegen
{

  void CodeGenerator::enterFuncDefFrame(ast::FuncDeclStmt &node)
  {
    frameLevels.push(0);
    currentScope = symbolTable.at(&node).get();

    std::set<std::string> paramNames;

    std::transform(node.params.cbegin(), node.params.cend(),
                   std::inserter(paramNames, paramNames.begin()),
                   [](const ast::FormalParam &param)
                   { return param.first; });

    std::map<std::string, FrameIndexMap::FrameIndex> frameIndices;
    int frameIndex = 0;

    for (auto const &[symbol, _] : node.params)
    {
      frameIndices.insert({symbol, frameIndex});
      frameIndex++;
    }

    // NOTE: Because of how the AST/semantic checking works rn, all symbols in
    // the currentScope will be parameters. However, accounting for the
    // possibility of extra variables in the scope makes the code less fragile
    // and susceptible to breaking if details of the AST/semantic checker are
    // changed.
    for (auto const &[symbol, entry] : currentScope->symbols)
    {
      // filter out function-type symbols and function params
      if (!entry->type->isFuncType() && !paramNames.count(symbol))
      {
        frameIndices.insert({symbol, frameIndex});
        frameIndex++;
      }
    }

    // NOTE: Fragile release/reset ordering that relies on applicative-order
    // evaluation.
    frameIndexMap.reset(
        new FrameIndexMap(std::move(frameIndices), frameIndexMap.release()));

    int allocSize = frameIndex - node.params.size();
    if (allocSize > 0)
    {
      addInstr(
          {PixIROpcode::PUSH, std::to_string(frameIndex - node.params.size())});
      addInstr({PixIROpcode::ALLOC});
    }
  }

  void CodeGenerator::exitFuncDefFrame()
  {
    frameLevels.pop();
    currentScope = currentScope->parent;

    frameIndexMap.reset(frameIndexMap->parent);
  }

  void CodeGenerator::enterMainFrame(ast::TranslationUnit &node)
  {
    frameLevels.push(0);
    currentScope = symbolTable.at(&node).get();

    std::map<std::string, FrameIndexMap::FrameIndex> frameIndices;
    int frameIndex = 0;

    for (auto &[symbol, entry] : currentScope->symbols)
    {
      // filter out function-type symbols
      if (!entry->type->isFuncType())
      {
        frameIndices.insert({symbol, frameIndex});
        frameIndex++;
      }
    }

    frameIndexMap.reset(
        new FrameIndexMap(std::move(frameIndices), frameIndexMap.release()));

    if (frameIndex > 0)
    {
      addInstr({PixIROpcode::PUSH, std::to_string(frameIndex)});
      addInstr({PixIROpcode::ALLOC});
    }
  }

  void CodeGenerator::exitMainFrame()
  {
    frameLevels.pop();
    currentScope = currentScope->parent;

    frameIndexMap.reset(frameIndexMap->parent);

    addInstr({PixIROpcode::HALT});
  }

  // what are called frames in the VM correspond to scopes in the
  // SemanticVisitor.
  void CodeGenerator::enterFrame(ast::StmtNode *stmt)
  {
    ++frameLevels.top();
    currentScope = symbolTable.at(stmt).get();

    std::map<std::string, FrameIndexMap::FrameIndex> frameIndices;
    int frameIndex = 0;

    for (auto &[symbol, entry] : currentScope->symbols)
    {
      // filter out function-type symbols
      if (!entry->type->isFuncType())
      {
        frameIndices.insert({symbol, frameIndex});
        frameIndex++;
      }
    }

    frameIndexMap.reset(
        new FrameIndexMap(std::move(frameIndices), frameIndexMap.release()));

    addInstr({PixIROpcode::PUSH, std::to_string(frameIndex)});
    addInstr({PixIROpcode::OFRAME});
  }

  void CodeGenerator::exitFrame()
  {
    --frameLevels.top();
    currentScope = currentScope->parent;

    addInstr({PixIROpcode::CFRAME});

    frameIndexMap.reset(frameIndexMap->parent);
  }

  BasicBlock *CodeGenerator::terminateBlock()
  {
    BasicBlock *old = blockStack.top();
    blockStack.pop();

    PixIRFunction *currentFunc = old->parentFunc;
    currentFunc->blocks.push_back(
        std::make_unique<BasicBlock>(BasicBlock{currentFunc, {}}));

    blockStack.push((--(currentFunc->blocks.end()))->get());

    return old;
  }

  void CodeGenerator::beginFunc(std::string funcName)
  {
    pixIRCode.push_back(
        std::make_unique<PixIRFunction>(PixIRFunction{"." + funcName, {}}));
    PixIRFunction *func = (--pixIRCode.end())->get();

    func->blocks.push_back(std::make_unique<BasicBlock>(BasicBlock{func, {}}));
    blockStack.push((func->blocks.begin())->get());
  }

  void CodeGenerator::endFunc() { blockStack.pop(); }

  void CodeGenerator::visit(ast::BinaryExprNode &node)
  {
    rvisitChildren(&node);
    switch (node.op)
    {
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
      addInstr({PixIROpcode::AND});
      break;
    case ast::BinaryExprNode::BinaryOp::OR:
      addInstr({PixIROpcode::OR});
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
      addInstr({PixIROpcode::NEQ});
      break;
    case ast::BinaryExprNode::BinaryOp::GE:
      addInstr({PixIROpcode::GE});
      break;
    case ast::BinaryExprNode::BinaryOp::LE:
      addInstr({PixIROpcode::LE});
      break;
    }
  }

  void CodeGenerator::visit(ast::UnaryExprNode &node)
  {
    rvisitChildren(&node);
    switch (node.op)
    {
    case ast::UnaryExprNode::UnaryOp::NOT:
      addInstr({PixIROpcode::NOT});
      break;
    case ast::UnaryExprNode::UnaryOp::MINUS:
      addInstr({PixIROpcode::PUSH, "0"});
      addInstr({PixIROpcode::SUB});
      break;
    }
  }

  void CodeGenerator::visit(ast::FunctionCallNode &node)
  {
    rvisitChildren(&node);
    addInstr({PixIROpcode::PUSH, std::to_string(node.children().size())});
    addInstr({PixIROpcode::PUSH, "." + node.funcName});
    addInstr({PixIROpcode::CALL});
  }

  void CodeGenerator::visit(ast::IdExprNode &node)
  {
    auto [depth, index] = frameIndexMap->getDepthAndIndex(node.id);

    if (node.isLValue)
    {
      addInstr({PixIROpcode::PUSH, std::to_string(index)});
      addInstr({PixIROpcode::PUSH, std::to_string(depth)});
    }
    else
    {
      addInstr({PixIROpcode::PUSH,
                "[" + std::to_string(index) +
                    (depth == 0 ? "" : ":" + std::to_string(depth)) + "]"});
    }
  }

  void CodeGenerator::visit(ast::BoolLiteralExprNode &node)
  {
    if (node.x)
    {
      addInstr({PixIROpcode::PUSH, "1"});
    }
    else
    {
      addInstr({PixIROpcode::PUSH, "0"});
    }
  }

  void CodeGenerator::visit(ast::IntLiteralExprNode &node)
  {
    addInstr({PixIROpcode::PUSH, std::to_string(node.x)});
  }

  void CodeGenerator::visit(ast::FloatLiteralExprNode &node)
  {
    addInstr({PixIROpcode::PUSH, std::to_string(node.x)});
  }

  void CodeGenerator::visit(ast::ColourLiteralExprNode &node)
  {
    std::stringstream ss;
    ss << "#" << std::hex << std::setw(6) << std::setfill('0') << node.colour;
    addInstr({PixIROpcode::PUSH, ss.str()});
  }

  void CodeGenerator::visit(ast::PadWidthExprNode &)
  {
    addInstr({PixIROpcode::WIDTH});
  }

  void CodeGenerator::visit(ast::PadHeightExprNode &)
  {
    addInstr({PixIROpcode::HEIGHT});
  }

  void CodeGenerator::visit(ast::ReadExprNode &node)
  {
    rvisitChildren(&node);
    addInstr({PixIROpcode::READ});
  }

  void CodeGenerator::visit(ast::RandiExprNode &node)
  {
    rvisitChildren(&node);
    addInstr({PixIROpcode::IRND});
  }

  void CodeGenerator::visit(ast::NewArrExprNode &node)
  {
    rvisitChildren(&node);
    addInstr({PixIROpcode::ALLOCA});
  }

  void CodeGenerator::visit(ast::ArrayAccessNode &node)
  {
    rvisitChildren(&node);
    addInstr({PixIROpcode::LDA});
  }

  void CodeGenerator::visit(ast::AssignmentStmt &node)
  {
    rvisitChildren(&node);
    if (dynamic_cast<ast::ArrayAccessNode *>(node.lvalue.get()) != nullptr)
    {
      popInstr();
      addInstr({PixIROpcode::STA});
    }
    else if (dynamic_cast<ast::IdExprNode *>(node.lvalue.get()) != nullptr)
    {
      addInstr({PixIROpcode::ST});
    }
  }

  // nothing to generate here. Space for variables is allocated when entering a
  // BlockStmt.
  void CodeGenerator::visit(ast::VariableDeclStmt &node)
  {
    // NOTE: unsafe unwrapping here because SemanticVisitor has already dealt
    // with error case.
    auto [depth, index] = frameIndexMap->getDepthAndIndex(node.id);

    rvisitChildren(&node);
    addInstr({PixIROpcode::PUSH, std::to_string(index)});
    addInstr({PixIROpcode::PUSH, std::to_string(depth)});
    addInstr({PixIROpcode::ST});
  }

  void CodeGenerator::visit(ast::PrintStmt &node)
  {
    rvisitChildren(&node);
    addInstr({PixIROpcode::PRINT});
  }

  void CodeGenerator::visit(ast::DelayStmt &node)
  {
    rvisitChildren(&node);
    addInstr({PixIROpcode::DELAY});
  }

  void CodeGenerator::visit(ast::PixelStmt &node)
  {
    rvisitChildren(&node);
    addInstr({PixIROpcode::PIXEL});
  }

  void CodeGenerator::visit(ast::PixelRStmt &node)
  {
    rvisitChildren(&node);
    addInstr({PixIROpcode::PIXELR});
  }

  void CodeGenerator::visit(ast::ReturnStmt &node)
  {
    rvisitChildren(&node);
    for (int i = frameLevels.top(); i > 0; i--)
    {
      addInstr({PixIROpcode::CFRAME});
    }
    addInstr({PixIROpcode::RET});
  }

  void CodeGenerator::visit(ast::IfElseStmt &node)
  {
    BasicBlock *head, *ifBlock, *elseBlock, *after;

    terminateBlock();
    node.cond->accept(this);

    head = terminateBlock();
    if (node.elseBody != nullptr)
    {
      node.elseBody->accept(this);
    }

    elseBlock = terminateBlock();

    ifBlock = blockStack.top();
    node.ifBody->accept(this);
    terminateBlock();

    after = blockStack.top();

    head->instrs.push_back({PixIROpcode::PUSH, ifBlock});
    head->instrs.push_back({PixIROpcode::CJMP2});

    elseBlock->instrs.push_back({PixIROpcode::PUSH, after});
    elseBlock->instrs.push_back({PixIROpcode::JMP});
  }

  void CodeGenerator::visit(ast::ForStmt &node)
  {
    enterFrame(&node);

    BasicBlock *head, *body, *after;

    // loop entry
    terminateBlock();
    node.varDecl->accept(this);

    terminateBlock();
    node.cond->accept(this);
    // !node.cond.
    addInstr({PixIROpcode::PUSH, "1"});
    addInstr({PixIROpcode::SUB});

    if (opts.rotateLoops)
    {
      // rotated loop body
      head = terminateBlock();
      body = blockStack.top();
      node.body->accept(this);
      node.assignment->accept(this);

      node.cond->accept(this);
      addInstr({PixIROpcode::PUSH, body});
      addInstr({PixIROpcode::CJMP2});
    }
    else
    {
      // regular loop body
      head = terminateBlock();
      node.body->accept(this);
      node.assignment->accept(this);
      addInstr({PixIROpcode::PUSH, head});
      addInstr({PixIROpcode::JMP});
    }

    // after block
    terminateBlock();
    after = blockStack.top();

    head->instrs.push_back({PixIROpcode::PUSH, after});
    head->instrs.push_back({PixIROpcode::CJMP2});

    exitFrame();
  }

  void CodeGenerator::visit(ast::WhileStmt &node)
  {
    BasicBlock *head, *body, *after;

    terminateBlock();
    node.cond->accept(this);
    // !node.cond.
    addInstr({PixIROpcode::PUSH, "1"});
    addInstr({PixIROpcode::SUB});

    if (opts.rotateLoops)
    {
      // rotated loop body
      head = terminateBlock();
      body = blockStack.top();
      node.body->accept(this);

      node.cond->accept(this);
      addInstr({PixIROpcode::PUSH, body});
      addInstr({PixIROpcode::CJMP2});
    }
    else
    {
      // regular loop body
      head = terminateBlock();
      node.body->accept(this);
      addInstr({PixIROpcode::PUSH, head});
      addInstr({PixIROpcode::JMP});
    }

    // after block
    terminateBlock();
    after = blockStack.top();

    head->instrs.push_back({PixIROpcode::PUSH, after});
    head->instrs.push_back({PixIROpcode::CJMP2});
  }

  void CodeGenerator::visit(ast::FuncDeclStmt &node)
  {
    beginFunc(node.funcName);
    enterFuncDefFrame(node);
    visitChildren(&node);
    exitFuncDefFrame();
    endFunc();
  }

  void CodeGenerator::visit(ast::BlockStmt &node)
  {
    enterFrame(&node);
    visitChildren(&node);
    exitFrame();
  }

  void CodeGenerator::visit(ast::TranslationUnit &node)
  {
    beginFunc(MAIN_FUNC_NAME);
    enterMainFrame(node);
    visitChildren(&node);
    exitMainFrame();
    endFunc();
  }

  void linearizeCode(PixIRCode &pixIRCode)
  {
    for (std::unique_ptr<PixIRFunction> &func : pixIRCode)
    {
      int offset = 0;
      std::map<BasicBlock *, int> offsets;

      // compute local offsets for each block
      for (std::unique_ptr<BasicBlock> &block : func->blocks)
      {
        offsets.insert({block.get(), offset});
        offset += block->instrs.size();
      }

      // use local offsets to convert BasicBlock * references in push instructions
      // to PC offsets.
      for (std::unique_ptr<BasicBlock> &block : func->blocks)
      {
        auto instrs_it = block->instrs.begin();
        for (size_t i = 0; instrs_it != block->instrs.end(); i++, ++instrs_it)
        {
          PixIRInstruction &instr = *instrs_it;
          if (instr.opcode == PixIROpcode::PUSH &&
              std::holds_alternative<BasicBlock *>(instr.data))
          {
            int pc_offset = offsets.at(std::get<BasicBlock *>(instr.data)) -
                            offsets.at(block.get()) - i;
            instr.data = std::string("#PC") + (pc_offset >= 0 ? "+" : "") +
                         std::to_string(pc_offset);
          }
        }
      }

      // remove empty blocks in one pass. This works because an empty block has
      // the same offset as the next block.
      for (auto it = func->blocks.begin(); it != func->blocks.end(); ++it)
      {
        if ((*it)->instrs.size() == 0)
        {
          --it;
          func->blocks.erase(it + 1);
        }
      }
    }
  }

  void dumpCode(PixIRCode &pixIRCode, std::ostream &s)
  {
    for (const std::unique_ptr<codegen::PixIRFunction> &func : pixIRCode)
    {
      s << func->funcName << std::endl;
      for (const std::unique_ptr<codegen::BasicBlock> &block : func->blocks)
      {
        for (const codegen::PixIRInstruction &instr : block->instrs)
        {
          s << "\t" << instr.to_string() << std::endl;
        }
        s << std::endl;
      }
    }
  }

  std::string to_string(const PixIROpcode type)
  {
    switch (type)
    {
    case AND:
      return "and";
    case OR:
      return "or";
    case NOT:
      return "not";
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
    case NEQ:
      return "neq";
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
    case ALLOCA:
      return "alloca";
    case STA:
      return "sta";
    case LDA:
      return "lda";
    }
    return ""; // please compiler
  }

} // end namespace codegen
