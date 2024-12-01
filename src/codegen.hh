#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "ast.hh"
#include "semantic_visitor.hh"
#include "visitor.hh"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <stdexcept>
#include <variant>
#include <vector>

namespace codegen
{

#define MAIN_FUNC_NAME "main"

  enum PixIROpcode
  {
    AND,
    OR,
    NOT,
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
    NEQ,
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
    // array operations
    ALLOCA,
    STA,
    LDA,
  };

  std::string to_string(const PixIROpcode type);

  struct BasicBlock;

  struct PixIRInstruction
  {
    PixIROpcode opcode;
    std::variant<std::monostate, BasicBlock *, std::string> data =
        std::monostate(); // only used for PUSH instruction

    std::string to_string() const
    {
      std::string result = codegen::to_string(opcode);
      if (std::holds_alternative<std::string>(data))
      {
        result += " " + std::get<std::string>(data);
      }
      else if (std::holds_alternative<BasicBlock *>(data))
      {
        throw std::logic_error(
            "Can't convert instruction with unresolved jump to string.");
      }
      return result;
    }
  };

  struct PixIRFunction;

  struct BasicBlock
  {
    PixIRFunction *parentFunc;
    std::list<PixIRInstruction> instrs;
  };

  struct PixIRFunction
  {
    std::string funcName;
    // unique_ptr is used so we can reference blocks without worrying about the
    // vector reallocating its memory.
    std::vector<std::unique_ptr<BasicBlock>> blocks;
  };

  // std::unique_ptr is used so we can reference functions without worrying
  // about the address changing due to vector reallocation
  using PixIRCode = std::vector<std::unique_ptr<PixIRFunction>>;

  struct FrameIndexMap
  {
    using FrameIndex = int;

    std::map<std::string, FrameIndex> frameIndices;
    FrameIndexMap *parent;

    FrameIndexMap(std::map<std::string, FrameIndex> &&frameIndices,
                  FrameIndexMap *parent = nullptr)
        : frameIndices(std::move(frameIndices)), parent(parent) {}

    // gets the depth (number of scopes traversed to obtain the symbol) and
    // index (in the std::map) of a symbol.
    std::pair<int, FrameIndex> getDepthAndIndex(std::string symbol,
                                                int initDepth = 0) const
    {
      if (frameIndices.count(symbol))
      {
        return {initDepth, frameIndices.at(symbol)};
      }
      if (parent != nullptr)
      {
        return parent->getDepthAndIndex(symbol, initDepth + 1);
      }
      throw std::logic_error("Symbol " + symbol + " not found");
    }
  };

  struct CodeGeneratorOptions
  {
    bool rotateLoops = false;
  };

  class CodeGenerator : public ast::AbstractVisitor
  {
  private:
    CodeGeneratorOptions opts;

    const ast::SymbolTable &symbolTable;

    PixIRCode pixIRCode;

    // scratch space for the generator
    std::stack<BasicBlock *> blockStack;
    std::unique_ptr<FrameIndexMap> frameIndexMap;

    std::stack<int> frameLevels;

    // current Scope and frame number.
    const ast::Scope *currentScope;

    void addInstr(PixIRInstruction instr)
    {
      blockStack.top()->instrs.push_back(instr);
    }

    void popInstr() { blockStack.top()->instrs.pop_back(); }

    void enterFuncDefFrame(ast::FuncDeclStmt &node);
    void exitFuncDefFrame();

    void enterMainFrame(ast::TranslationUnit &node);
    void exitMainFrame();

    // what are called frames in the VM correspond to scopes in the
    // SemanticVisitor.
    void enterFrame(ast::StmtNode *stmt);
    void exitFrame();

    BasicBlock *terminateBlock();

    void beginFunc(std::string funcName);
    void endFunc();

  public:
    CodeGenerator(const ast::SymbolTable &symbolTable,
                  CodeGeneratorOptions &&opts)
        : opts(std::move(opts)), symbolTable(symbolTable) {}

    // nothing to generate for types.
    void visit(ast::IntTypeNode &node) override {}
    void visit(ast::FloatTypeNode &node) override {}
    void visit(ast::ColourTypeNode &node) override {}
    void visit(ast::BoolTypeNode &node) override {}
    void visit(ast::ArrayTypeNode &node) override {}

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
    void visit(ast::NewArrExprNode &node) override;
    void visit(ast::ArrayAccessNode &node) override;

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
