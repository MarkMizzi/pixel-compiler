#ifndef SEMANTIC_VISITOR_H_
#define SEMANTIC_VISITOR_H_

#include "ast.hh"
#include "visitor.hh"

#include <map>
#include <memory>
#include <optional>

namespace ast {

enum SemanticType {
  INT_TYPE,
  COLOUR_TYPE,
  FLOAT_TYPE,
  BOOL_TYPE,

  // allows us to keep track of locally defined functions as symbols in the
  // symbol table
  FUNCTION_TYPE,
};

SemanticType typenameToSemanticType(Typename type);

struct SymbolTableEntry {
  SemanticType type;
};

using SymbolTable = std::map<std::string, SymbolTableEntry>;

struct Scope {
  SymbolTable symbols;
  Scope *parent = nullptr;

  Scope(SymbolTable &&symbols, Scope *parent)
      : symbols(std::move(symbols)), parent(parent) {}

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
    symbols[symbol] = entry;
  }
};

class SemanticVisitor : public AbstractVisitor {
private:
  std::unique_ptr<Scope> currentScope = nullptr;

  void enterScope() {
    currentScope =
        std::make_unique<Scope>(SymbolTable{}, currentScope.release());
  }

  void exitScope() { currentScope.reset(currentScope->parent); }

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
