#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "ast.hh"
#include "semantic_visitor.hh"
#include "visitor.hh"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <stdexcept>
#include <variant>
#include <vector>

namespace codegen {

#define MAIN_FUNC_NAME "main"

enum PixIROpcode {
  ADD,
  SUB,
  MUL,
  DIV,
  INC,
  DEC,
  MAX,
  MIN,
  IRND,
  LT,
  LE,
  EQ,
  GT,
  GE,
  PUSH,
  JMP,
  CJMP,
  CJMP2,
  CALL,
  RET,
  ST,
  ALLOC,
  OFRAME,
  CFRAME,
  DELAY,
  PIXEL,
  PIXELR,
  CLEAR,
  READ,
  WIDTH,
  HEIGHT,
  PRINT,
  DUP,
  HALT,
};

std::string to_string(const PixIROpcode type);

struct BasicBlock;

struct PixIRInstruction {
  PixIROpcode opcode;
  std::variant<std::monostate, BasicBlock *, std::string> data =
      std::monostate(); // only used for PUSH instruction

  std::string to_string() const {
    std::string result = codegen::to_string(opcode);
    if (std::holds_alternative<std::string>(data)) {
      result += " " + std::get<std::string>(data);
    } else if (std::holds_alternative<BasicBlock *>(data)) {
      throw std::logic_error(
          "Can't convert instruction with unresolved jump to string.");
    }
    return result;
  }
};

struct PixIRFunction;

struct BasicBlock {
  PixIRFunction *parentFunc;
  std::vector<PixIRInstruction> instrs;
};

struct PixIRFunction {
  std::string funcName;
  // unique_ptr is used so we can reference blocks without worrying about the
  // vector reallocating its memory.
  std::vector<std::unique_ptr<BasicBlock>> blocks;
};

// std::unique_ptr is used so we can reference functions without worrying
// about the address changing due to vector reallocation
using PixIRCode = std::vector<std::unique_ptr<PixIRFunction>>;

struct SymbolFrameIndexMap {
  using FrameIndex = int;

  std::map<std::string, FrameIndex> frameIndices;
  SymbolFrameIndexMap *parent;

  SymbolFrameIndexMap(std::map<std::string, FrameIndex> &&frameIndices,
                      SymbolFrameIndexMap *parent = nullptr)
      : frameIndices(std::move(frameIndices)), parent(parent) {}

  // gets the depth (number of scopes traversed to obtain the symbol) and
  // index (in the std::map) of a symbol.
  std::pair<int, FrameIndex> getDepthAndIndex(std::string symbol,
                                              int initDepth = 0) const {
    if (frameIndices.count(symbol)) {
      return {initDepth, frameIndices.at(symbol)};
    }
    if (parent != nullptr) {
      return parent->getDepthAndIndex(symbol, initDepth + 1);
    }
    throw std::logic_error("Symbol " + symbol + " not found");
  }
};

class CodeGenerator : public ast::AbstractVisitor {
private:
  const ast::SymbolTable &symbolTable;

  PixIRCode pixIRCode;

  // scratch space for the generator
  std::stack<BasicBlock *> blockStack;
  std::unique_ptr<SymbolFrameIndexMap> frameIndexMap;

  // current Scope and frame number.
  const ast::Scope *currentScope;

  void addInstr(PixIRInstruction instr) {
    blockStack.top()->instrs.push_back(instr);
  }

  void enterFuncDefFrame(ast::FuncDeclStmt &node) {
    currentScope = symbolTable.at(&node).get();

    std::set<std::string> paramNames;

    std::transform(node.params.cbegin(), node.params.cend(),
                   std::inserter(paramNames, paramNames.begin()),
                   [](const ast::FormalParam &param) { return param.first; });

    std::map<std::string, SymbolFrameIndexMap::FrameIndex> frameIndices;
    int frameIndex = 0;

    for (auto const &[symbol, _] : node.params) {
      frameIndices.insert({symbol, frameIndex});
      frameIndex++;
    }

    // NOTE: Because of how the AST/semantic checking works rn, all symbols in
    // the currentScope will be parameters. However, accounting for the
    // possibility of extra variables in the scope makes the code less fragile
    // and susceptible to breaking if details of the AST/semantic checker are
    // changed.
    for (auto &[symbol, entry] : currentScope->symbols) {
      // filter out function-type symbols and function params
      if (!entry.type.isFunctionType() && !paramNames.count(symbol)) {
        frameIndices.insert({symbol, frameIndex});
        frameIndex++;
      }
    }

    // NOTE: Fragile release/reset ordering that relies on applicative-order
    // evaluation.
    frameIndexMap.reset(new SymbolFrameIndexMap(std::move(frameIndices),
                                                frameIndexMap.release()));

    int allocSize = frameIndex - node.params.size();
    if (allocSize > 0) {
      addInstr(
          {PixIROpcode::PUSH, std::to_string(frameIndex - node.params.size())});
      addInstr({PixIROpcode::ALLOC});
    }
  }

  void exitFuncDefFrame() {
    currentScope = currentScope->parent;

    frameIndexMap.reset(frameIndexMap->parent);
  }

  void enterMainFrame(ast::TranslationUnit &node) {
    currentScope = symbolTable.at(&node).get();

    std::map<std::string, SymbolFrameIndexMap::FrameIndex> frameIndices;
    int frameIndex = 0;

    for (auto &[symbol, entry] : currentScope->symbols) {
      // filter out function-type symbols
      if (!entry.type.isFunctionType()) {
        frameIndices.insert({symbol, frameIndex});
        frameIndex++;
      }
    }

    frameIndexMap.reset(new SymbolFrameIndexMap(std::move(frameIndices),
                                                frameIndexMap.release()));

    if (frameIndex > 0) {
      addInstr({PixIROpcode::PUSH, std::to_string(frameIndex)});
      addInstr({PixIROpcode::ALLOC});
    }
  }

  void exitMainFrame() {
    currentScope = currentScope->parent;

    frameIndexMap.reset(frameIndexMap->parent);

    addInstr({PixIROpcode::HALT});
  }

  // what are called frames in the VM correspond to scopes in the
  // SemanticVisitor.
  void enterFrame(ast::StmtNode *stmt) {
    currentScope = symbolTable.at(stmt).get();

    std::map<std::string, SymbolFrameIndexMap::FrameIndex> frameIndices;
    int frameIndex = 0;

    for (auto &[symbol, entry] : currentScope->symbols) {
      // filter out function-type symbols
      if (!entry.type.isFunctionType()) {
        frameIndices.insert({symbol, frameIndex});
        frameIndex++;
      }
    }

    frameIndexMap.reset(new SymbolFrameIndexMap(std::move(frameIndices),
                                                frameIndexMap.release()));

    addInstr({PixIROpcode::PUSH, std::to_string(frameIndex)});
    addInstr({PixIROpcode::OFRAME});
  }

  void exitFrame() {
    currentScope = currentScope->parent;

    addInstr({PixIROpcode::CFRAME});

    frameIndexMap.reset(frameIndexMap->parent);
  }

  BasicBlock *terminateBlock() {
    BasicBlock *old = blockStack.top();
    blockStack.pop();

    PixIRFunction *currentFunc = old->parentFunc;
    currentFunc->blocks.push_back(
        std::make_unique<BasicBlock>(BasicBlock{currentFunc, {}}));

    blockStack.push((--(currentFunc->blocks.end()))->get());

    return old;
  }

  void beginFunc(std::string funcName) {
    pixIRCode.push_back(
        std::make_unique<PixIRFunction>(PixIRFunction{"." + funcName, {}}));
    PixIRFunction *func = (--pixIRCode.end())->get();

    func->blocks.push_back(std::make_unique<BasicBlock>(BasicBlock{func, {}}));
    blockStack.push((func->blocks.begin())->get());
  }

  void endFunc() { blockStack.pop(); }

public:
  CodeGenerator(const ast::SymbolTable &symbolTable)
      : symbolTable(symbolTable) {}

  void visit(ast::BinaryExprNode &node) override;
  void visit(ast::UnaryExprNode &node) override;
  void visit(ast::FunctionCallNode &node) override;
  void visit(ast::IdExprNode &node) override;
  void visit(ast::BoolLiteralExprNode &node) override;
  void visit(ast::IntLiteralExprNode &node) override;
  void visit(ast::FloatLiteralExprNode &node) override;
  void visit(ast::ColourLiteralExprNode &node) override;
  void visit(ast::PadWidthExprNode &node) override;
  void visit(ast::PadHeightExprNode &node) override;
  void visit(ast::ReadExprNode &node) override;
  void visit(ast::RandiExprNode &node) override;

  void visit(ast::AssignmentStmt &node) override;
  void visit(ast::VariableDeclStmt &node) override;
  void visit(ast::PrintStmt &node) override;
  void visit(ast::DelayStmt &node) override;
  void visit(ast::PixelStmt &node) override;
  void visit(ast::PixelRStmt &node) override;
  void visit(ast::ReturnStmt &node) override;
  void visit(ast::IfElseStmt &node) override;
  void visit(ast::ForStmt &node) override;
  void visit(ast::WhileStmt &node) override;
  void visit(ast::FuncDeclStmt &node) override;
  void visit(ast::BlockStmt &node) override;

  void visit(ast::TranslationUnit &node) override;

  PixIRCode &code() { return pixIRCode; }
};

// Multiple responsibilities:
// 1. convert BasicBlock references in PUSH instructions to PC offsets
// 2. remove empty blocks produced in code generation.
void linearizeCode(PixIRCode &pixIRCode);
void dumpCode(PixIRCode &pixIRCode, std::ostream &s);

} // namespace codegen

#endif // CODEGEN_H_
