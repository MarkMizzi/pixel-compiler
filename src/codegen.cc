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

} // end namespace codegen
