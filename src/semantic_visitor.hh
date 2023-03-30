#ifndef SEMANTIC_VISITOR_H_
#define SEMANTIC_VISITOR_H_

#include "ast.hh"
#include "visitor.hh"

#include <algorithm>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

namespace ast {

class SemanticError : public std::runtime_error {
public:
  SemanticError(std::string errmsg, Location loc)
      : std::runtime_error("Semantic error at [" + std::to_string(loc.sline) +
                           ":" + std::to_string(loc.scol) + "]-[" +
                           std::to_string(loc.eline) + ":" +
                           std::to_string(loc.ecol) + "]: " + errmsg) {}
};

using SemanticFunctionType = std::pair<Typename, std::vector<Typename>>;

class SemanticType {
private:
  std::variant<Typename, SemanticFunctionType> semanticType;

public:
  SemanticType(Typename type) : semanticType(type) {}
  SemanticType(SemanticFunctionType type) : semanticType(type) {}

  std::string to_string() const {
    if (std::holds_alternative<Typename>(semanticType)) {
      return ast::to_string(std::get<Typename>(semanticType));
    } else {
      auto const &[retType, argTypes] =
          std::get<SemanticFunctionType>(semanticType);
      std::string result = ast::to_string(retType);

      std::vector<std::string> xs(argTypes.size());
      std::transform(argTypes.begin(), argTypes.end(), xs.begin(),
                     [](const Typename &type) { return ast::to_string(type); });

      return result + "(" +
             std::accumulate(xs.begin(), xs.end(), std::string(),
                             [](const std::string &x, const std::string &y) {
                               return x + ", " + y;
                             }) +
             ")";
    }
  }

  bool operator==(const SemanticType &other) const {
    return this->semanticType == other.semanticType;
  }

  bool operator!=(const SemanticType &other) const {
    return this->semanticType != other.semanticType;
  }

  const SemanticFunctionType *getFunctionType() const {
    return std::get_if<SemanticFunctionType>(&semanticType);
  }
};

struct SymbolTableEntry {
  SemanticType type;
};

using SymbolTable = std::map<std::string, SymbolTableEntry>;

struct Scope {
  SymbolTable symbols;
  Scope *parent = nullptr;
  // stores the signature of a function whose scope we are entering.
  std::optional<SemanticFunctionType> funcType;

  Scope(SymbolTable &&symbols, Scope *parent,
        std::optional<SemanticFunctionType> funcType = std::nullopt)
      : symbols(std::move(symbols)), parent(parent), funcType(funcType) {}

  // fetches the signature of the current scope's function (if any)
  std::optional<SemanticFunctionType> getFuncType() {
    if (funcType.has_value()) {
      return funcType;
    }
    if (parent != nullptr) {
      return parent->getFuncType();
    }
    return std::nullopt;
  }

  std::optional<SymbolTableEntry> get(std::string symbol) {
    if (symbols.count(symbol)) {
      return symbols.at(symbol);
    }
    if (parent != nullptr) {
      return parent->get(symbol);
    }
    return std::nullopt;
  }

  void add(std::string symbol, SymbolTableEntry entry) {
    symbols.insert({symbol, entry});
  }
};

class SemanticVisitor : public AbstractVisitor {
private:
  std::unique_ptr<Scope> currentScope = nullptr;

  // scratch table for the type checker. Used for keeping track of types of
  // subexpressions while type-checking a compound expression.
  //
  // Torn down every time a new scope is entered or exited to save on memory.
  // That's fine even with nested scopes, since we don't have nested scopes
  // *INSIDE* an expression.
  //
  // We could even be more fine-grained and tear down in the visit*() methods,
  // but this makes the implementation more complex.
  std::map<ExprNode *, SemanticType> typeCheckerTable;

  void enterScope(std::optional<SemanticFunctionType> funcType = std::nullopt) {
    currentScope = std::make_unique<Scope>(SymbolTable{},
                                           currentScope.release(), funcType);
    typeCheckerTable.clear();
  }

  void exitScope() {
    currentScope.reset(currentScope->parent);
    typeCheckerTable.clear();
  }

public:
  void visit(BinaryExprNode &node) override;
  void visit(UnaryExprNode &node) override;
  void visit(FunctionCallNode &node) override;
  void visit(IdExprNode &node) override;
  void visit(BoolLiteralExprNode &node) override;
  void visit(IntLiteralExprNode &node) override;
  void visit(FloatLiteralExprNode &node) override;
  void visit(ColourLiteralExprNode &node) override;
  void visit(PadWidthExprNode &node) override;
  void visit(PadHeightExprNode &node) override;
  void visit(ReadExprNode &node) override;
  void visit(RandiExprNode &node) override;

  void visit(AssignmentStmt &node) override;
  void visit(VariableDeclStmt &node) override;
  void visit(PrintStmt &node) override;
  void visit(DelayStmt &node) override;
  void visit(PixelStmt &node) override;
  void visit(PixelRStmt &node) override;
  void visit(ReturnStmt &node) override;
  void visit(IfElseStmt &node) override;
  void visit(ForStmt &node) override;
  void visit(WhileStmt &node) override;
  void visit(FuncDeclStmt &node) override;
  void visit(BlockStmt &node) override;

  void visit(TranslationUnit &node) override;
};

} // namespace ast

#endif // SEMANTIC_VISITOR_H_
