#include "visitor.hh"
#include "ast.hh"

namespace ast {
void AbstractVisitor::visitChildren(ASTNode *node) {
  for (ASTNode *node : node->children()) {
    node->accept(this);
  }
}
} // namespace ast
