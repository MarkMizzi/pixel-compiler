#include "visitor.hh"
#include "ast.hh"

namespace ast {
void AbstractVisitor::visitChildren(ASTNode *node) {
  for (ASTNode *child : node->children()) {
    child->accept(this);
  }
}

void AbstractVisitor::rvisitChildren(ASTNode *node) {
  std::vector<ASTNode *> children = node->children();
  for (auto it = children.rbegin(); it != children.rend(); ++it) {
    (*it)->accept(this);
  }
}

} // namespace ast
