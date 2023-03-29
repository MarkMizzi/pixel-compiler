#include "ast.hh"

namespace ast {
std::string to_string(Typename type) {
  switch (type) {
  case BOOL:
    return "bool";
  case INT:
    return "int";
  case FLOAT:
    return "float";
  case COLOUR:
    return "colour";
  }
  return ""; // please compiler
}

} // namespace ast
