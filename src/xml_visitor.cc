#include "xml_visitor.hh"
#include "ast.hh"

namespace ast {

#define XML_ELEM_WITH_CHILDREN(NODE, TAGNAME, ATTRS)                           \
  ss << std::string(indent, ' ') << "<" << (TAGNAME) << ATTRS << " loc=\""     \
     << (NODE).loc.to_string()                                                 \
     << "\""                                                                   \
        ">"                                                                    \
     << std::endl;                                                             \
  indent++;                                                                    \
  visitChildren((ASTNode *)(&(NODE)));                                         \
  indent--;                                                                    \
  ss << std::string(indent, ' ') << "</" << (TAGNAME) << ">" << std::endl

#define XML_ELEM_WITH_CONTENT(NODE, TAGNAME, ATTRS, CONTENT)                   \
  ss << std::string(indent, ' ') << "<" << (TAGNAME) << ATTRS << " loc=\""     \
     << (NODE).loc.to_string() << "\""                                         \
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
    op = "&gt;";
    break;
  case BinaryExprNode::BinaryOp::LESS:
    op = "&lt;";
    break;
  case BinaryExprNode::BinaryOp::EQ:
    op = "==";
    break;
  case BinaryExprNode::BinaryOp::NEQ:
    op = "!=";
    break;
  case BinaryExprNode::BinaryOp::GE:
    op = "&gt;=";
    break;
  case BinaryExprNode::BinaryOp::LE:
    op = "&lt;=";
    break;
  }

  XML_ELEM_WITH_CHILDREN(node, "BinaryExprNode", " op=\"" << op << "\"");
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

  XML_ELEM_WITH_CHILDREN(node, "UnaryExprNode", " op=\"" << op << "\"");
}

void XMLVisitor::visit(FunctionCallNode &node) {
  XML_ELEM_WITH_CHILDREN(node, "FunctionCallNode",
                         " funcName=\"" << node.funcName << "\"");
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

void XMLVisitor::visit(AssignmentStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "AssignmentStmt", " id=\"" << node.id << "\"");
}

void XMLVisitor::visit(VariableDeclStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "VariableDeclStmt",
                         " id=\"" << node.id << "\" type=\""
                                  << to_string(node.type) << "\"");
}

void XMLVisitor::visit(PrintStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "PrintStmt", "");
}

void XMLVisitor::visit(DelayStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "DelayStmt", "");
}

void XMLVisitor::visit(PixelStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "PixelStmt", "");
}

void XMLVisitor::visit(PixelRStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "PixelRStmt", "");
}

void XMLVisitor::visit(ReturnStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "ReturnStmt", "");
}

void XMLVisitor::visit(IfElseStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "IfElseStmt", "");
}

void XMLVisitor::visit(ForStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "ForStmt", "");
}

void XMLVisitor::visit(WhileStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "WhileStmt", "");
}

void XMLVisitor::visit(FuncDeclStmt &node) {
  ss << std::string(indent, ' ') << "<FuncDeclStmt "
     << "loc=\"" << node.loc.to_string() << "\">" << std::endl;

  indent++;

  for (FormalParam param : node.params) {
    ss << std::string(indent, ' ') << "<FormalParam name=\"" << param.first
       << "\" type=\"" << to_string(param.second) << "\"></FormalParam>"
       << std::endl;
  }

  ss << std::string(indent, ' ') << "<Returns type=\""
     << to_string(node.retType) << "\"></Returns>" << std::endl;

  visitChildren(&node);

  indent--;

  ss << std::string(indent, ' ') << "</FuncDeclStmt>" << std::endl;
}

void XMLVisitor::visit(BlockStmt &node) {
  XML_ELEM_WITH_CHILDREN(node, "BlockStmt", "");
}

void XMLVisitor::visit(TranslationUnit &node) {
  XML_ELEM_WITH_CHILDREN(node, "TranslationUnit", "");
}

} // namespace ast
