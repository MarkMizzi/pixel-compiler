#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "ast.hh"
#include "visitor.hh"

#include <map>
#include <optional>
#include <vector>

namespace codegen {

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
  std::optional<std::string> data; // only for PUSH instruction

  std::string to_string() const {
    return codegen::to_string(type) +
           (data.has_value() ? " " + data.value() : "");
  }
};

using BasicBlock = std::vector<PixIRInstruction>;

class CodeGenerator : public ast::AbstractVisitor {
private:
  using ProgramCounter = int;

  std::map<ast::StmtNode *, ProgramCounter> jumpTable;
  std::vector<BasicBlock> pixIRCode;

public:
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

  const std::vector<BasicBlock> &code() { return pixIRCode; }
};

} // namespace codegen

#endif // CODEGEN_H_
