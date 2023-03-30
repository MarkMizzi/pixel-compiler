#include "semantic_visitor.hh"

#include <stdexcept>

namespace ast {

SemanticType typenameToSemanticType(Typename type) {
  switch (type) {
  case Typename::INT:
    return SemanticType::INT_TYPE;
  case Typename::BOOL:
    return SemanticType::BOOL_TYPE;
  case Typename::FLOAT:
    return SemanticType::FLOAT_TYPE;
  case Typename::COLOUR:
    return SemanticType::COLOUR_TYPE;
  }
  throw std::logic_error(
      "Impossible, everything was handled in the switch"); // please the
                                                           // compiler
}

} // end namespace ast
