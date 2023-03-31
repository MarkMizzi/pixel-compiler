#include "codegen.hh"

namespace codegen {

void CodeGenerator::visit(ast::BinaryExprNode &node) {}
void CodeGenerator::visit(ast::UnaryExprNode &node) {}
void CodeGenerator::visit(ast::FunctionCallNode &node) {}
void CodeGenerator::visit(ast::IdExprNode &node) {}
void CodeGenerator::visit(ast::BoolLiteralExprNode &node) {}
void CodeGenerator::visit(ast::IntLiteralExprNode &node) {}
void CodeGenerator::visit(ast::FloatLiteralExprNode &node) {}
void CodeGenerator::visit(ast::ColourLiteralExprNode &node) {}
void CodeGenerator::visit(ast::PadWidthExprNode &node) {}
void CodeGenerator::visit(ast::PadHeightExprNode &node) {}
void CodeGenerator::visit(ast::ReadExprNode &node) {}
void CodeGenerator::visit(ast::RandiExprNode &node) {}

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
