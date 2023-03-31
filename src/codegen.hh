#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "ast.hh"
#include "semantic_visitor.hh"
#include "visitor.hh"

#include <map>
#include <optional>
#include <stack>
#include <vector>

namespace codegen {

#define MAIN_FUNC_NAME ".main"

enum PixIRInstructionType {
  ADD,
  SUB,
  MUL,
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
};

std::string to_string(const PixIRInstructionType type);

struct PixIRInstruction {
  PixIRInstructionType type;
  std::optional<std::string> data = std::nullopt; // only for PUSH instruction

  std::string to_string() const {
    return codegen::to_string(type) +
           (data.has_value() ? " " + data.value() : "");
  }
};

struct PixIRFunction;

struct BasicBlock {
  PixIRFunction *parentFunc;
  std::vector<PixIRInstruction> instrs;
};

struct PixIRFunction {
  std::string funcName;
  std::vector<BasicBlock> blocks;
};

class CodeGenerator : public ast::AbstractVisitor {
private:
  const ast::SymbolTable &symbolTable;

  std::vector<PixIRFunction> pixIRCode;

  // scratch space for the generator
  std::map<ast::StmtNode *, BasicBlock &> jumpTable;
  std::stack<BasicBlock *> blockStack;

  void enterBasicBlock() {
    PixIRFunction *currentFunc = blockStack.top()->parentFunc;
    currentFunc->blocks.push_back(BasicBlock{currentFunc, {}});
    blockStack.push(&*--(currentFunc->blocks.end()));
  }

  void exitBasicBlock() { blockStack.pop(); }

  void enterFunc(std::string funcName) {
    pixIRCode.push_back(PixIRFunction{funcName, {}});
    PixIRFunction *func = &*--pixIRCode.end();

    BasicBlock entry{func, {}};

    func->blocks.push_back(std::move(entry));
    blockStack.push(&*(func->blocks.begin()));
  }

  void exitFunc() { blockStack.pop(); }

  void addInstr(PixIRInstruction instr) {
    blockStack.top()->instrs.push_back(instr);
  }

public:
  CodeGenerator(const ast::SymbolTable &symbolTable)
      : symbolTable(symbolTable) {

    enterFunc(MAIN_FUNC_NAME);
  }

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

  const std::vector<PixIRFunction> &code() { return pixIRCode; }
};

} // namespace codegen

#endif // CODEGEN_H_
