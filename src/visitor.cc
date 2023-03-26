#include "visitor.hh"
#include "ast.hh"

namespace parser {
void AbstractVisitor::visitChildren(ASTNode *node) {
  for (ASTNode *node : node->children()) {
    node->accept(this);
  }
}
} // end namespace parser
