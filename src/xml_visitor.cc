#include "xml_visitor.hh"
#include "ast.hh"

namespace ast {

#define XML_ELEM_WITH_CHILDREN(NODE, TAGNAME, ATTRS)                           \
  ss << std::string(' ', indent) << "<" << (TAGNAME) << " " << ATTRS           \
     << " loc=\"" << (NODE).loc.to_string()                                    \
     << "\""                                                                   \
        ">"                                                                    \
     << std::endl;                                                             \
  indent++;                                                                    \
  visitChildren((ASTNode *)(&(NODE)));                                         \
  indent--;                                                                    \
  ss << std::string(' ', indent) << "</" << (TAGNAME) << ">" << std::endl

#define XML_ELEM_WITH_CONTENT(NODE, TAGNAME, ATTRS, CONTENT)                   \
  ss << std::string(' ', indent) << "<" << (TAGNAME) << " " << ATTRS           \
     << " loc=\"" << (NODE).loc.to_string() << "\""                            \
     << ">" << (CONTENT) << "</" << (TAGNAME) << ">" << std::endl

void XMLVisitor::visit(BinaryExprNode &node) {
  std::string op;
  switch (node.op) {
  case BinaryExprNode::BinaryOp::ADD:
    op = "+";
    break;
  case BinaryExprNode::BinaryOp::SUB:
    op = "-";
    break;
  case BinaryExprNode::BinaryOp::DIV:
    op = "/";
    break;
  case BinaryExprNode::BinaryOp::MUL:
    op = "*";
    break;
  case BinaryExprNode::BinaryOp::AND:
    op = "and";
    break;
  case BinaryExprNode::BinaryOp::OR:
    op = "or";
    break;
  case BinaryExprNode::BinaryOp::GREATER:
    op = ">";
    break;
  case BinaryExprNode::BinaryOp::LESS:
    op = "<";
    break;
  case BinaryExprNode::BinaryOp::EQ:
    op = "==";
    break;
  case BinaryExprNode::BinaryOp::NEQ:
    op = "!=";
    break;
  case BinaryExprNode::BinaryOp::GE:
    op = ">=";
    break;
  case BinaryExprNode::BinaryOp::LE:
    op = "<=";
    break;
  }

  XML_ELEM_WITH_CHILDREN(node, "BinaryExprNode", "op = \"" << op << "\"");
}

void XMLVisitor::visit(UnaryExprNode &node) {
  std::string op;
  switch (node.op) {
  case UnaryExprNode::UnaryOp::MINUS:
    op = "-";
    break;
  case UnaryExprNode::UnaryOp::NOT:
    op = "not";
    break;
  }

  XML_ELEM_WITH_CHILDREN(node, "UnaryExprNode", "op = \"" << op << "\"");
}

void XMLVisitor::visit(FunctionCallNode &node) {
  XML_ELEM_WITH_CHILDREN(node, "FunctionCallNode",
                         "funcName = \"" << node.funcName << "\"");
}

void XMLVisitor::visit(IdExprNode &node) {
  XML_ELEM_WITH_CONTENT(node, "IdExprNode", "", node.id);
}

void XMLVisitor::visit(BoolLiteralExprNode &node) {
  XML_ELEM_WITH_CONTENT(node, "BoolLiteralExprNode", "",
                        std::to_string(node.x));
}

void XMLVisitor::visit(IntLiteralExprNode &node) {
  XML_ELEM_WITH_CONTENT(node, "IntLiteralExprNode", "", std::to_string(node.x));
}

void XMLVisitor::visit(FloatLiteralExprNode &node) {
  XML_ELEM_WITH_CONTENT(node, "FloatLiteralExprNode", "",
                        std::to_string(node.x));
}

void XMLVisitor::visit(ColourLiteralExprNode &node) {
  XML_ELEM_WITH_CONTENT(node, "ColourLiteralExprNode", "",
                        std::to_string(node.colour));
}

void XMLVisitor::visit(PadWidthExprNode &node) {
  XML_ELEM_WITH_CONTENT(node, "PadWidthExprNode", "", "");
}

void XMLVisitor::visit(PadHeightExprNode &node) {
  XML_ELEM_WITH_CONTENT(node, "PadHeightExprNode", "", "");
}

void XMLVisitor::visit(ReadExprNode &node) {
  XML_ELEM_WITH_CHILDREN(node, "ReadExprNode", "");
}

void XMLVisitor::visit(RandiExprNode &node) {
  XML_ELEM_WITH_CHILDREN(node, "RandiExprNode", "");
}

void XMLVisitor::visit(AssignmentStmt &node) {}
void XMLVisitor::visit(VariableDeclStmt &node) {}
void XMLVisitor::visit(PrintStmt &node) {}
void XMLVisitor::visit(DelayStmt &node) {}
void XMLVisitor::visit(PixelStmt &node) {}
void XMLVisitor::visit(PixelRStmt &node) {}
void XMLVisitor::visit(ReturnStmt &node) {}
void XMLVisitor::visit(IfElseStmt &node) {}
void XMLVisitor::visit(ForStmt &node) {}
void XMLVisitor::visit(WhileStmt &node) {}
void XMLVisitor::visit(FuncDeclStmt &node) {}
void XMLVisitor::visit(BlockStmt &node) {}

void XMLVisitor::visit(TranslationUnit &node) {}

} // namespace ast
