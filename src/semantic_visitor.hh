#ifndef SEMANTIC_VISITOR_H_
#define SEMANTIC_VISITOR_H_

#include "ast.hh"
#include "util.hh"
#include "visitor.hh"

#include <map>
#include <memory>
#include <optional>
#include <stack>

namespace ast
{

  class SemanticError : public CompilationError
  {
  public:
    SemanticError(std::string errmsg, Location loc)
        : CompilationError("Semantic error at [" + std::to_string(loc.sline) +
                           ":" + std::to_string(loc.scol) + "]-[" +
                           std::to_string(loc.eline) + ":" +
                           std::to_string(loc.ecol) + "]: " + errmsg) {}
  };

  struct SymbolTableEntry
  {
    TypeNodePtr type;

    SymbolTableEntry(TypeNodePtr &&type) : type(std::move(type)) {}
  };

  struct Scope
  {
    std::map<std::string, std::unique_ptr<SymbolTableEntry>> symbols;
    Scope *parent = nullptr;
    // stores the signature of a function whose scope we are entering.
    std::optional<FunctionTypeNode> funcType;

    Scope(std::map<std::string, std::unique_ptr<SymbolTableEntry>> &&symbols,
          Scope *parent,
          std::optional<FunctionTypeNode> &&funcType = std::nullopt)
        : symbols(std::move(symbols)), parent(parent),
          funcType(std::move(funcType)) {}

    // fetches the signature of the current scope's function (if any)
    const std::optional<FunctionTypeNode> &getFuncType() const
    {
      if (funcType.has_value())
      {
        return funcType;
      }
      if (parent != nullptr)
      {
        return parent->getFuncType();
      }
      return funcType; // funcType == std::nullopt if we reached this point.
    }

    const SymbolTableEntry *get(std::string symbol) const
    {
      if (symbols.count(symbol))
      {
        return symbols.at(symbol).get();
      }
      if (parent != nullptr)
      {
        return parent->get(symbol);
      }
      return nullptr;
    }

    void add(std::string symbol, std::unique_ptr<SymbolTableEntry> &&entry)
    {
      symbols.insert({symbol, std::move(entry)});
    }
  };

  using SymbolTable = std::map<StmtNode *, std::unique_ptr<Scope>>;

  class SemanticVisitor : public AbstractVisitor
  {
  private:
    SymbolTable &symbolTable;
    Scope *currentScope = nullptr;

    using TypeCheckerTable = std::map<const ExprNode *, TypeNodePtr>;
    // scratch tables for the type checker. Used for keeping track of types of
    // subexpressions while type-checking a compound expression.
    //
    // We have a stack of them so we can tear down old tables when we exit a
    // scope.
    std::stack<TypeCheckerTable> typeCheckerTables;

    TypeCheckerTable &typeCheckerTable() { return typeCheckerTables.top(); }

    void enterScope(StmtNode *stmt,
                    std::optional<FunctionTypeNode> &&funcType = std::nullopt)
    {
      symbolTable.insert({stmt, std::make_unique<Scope>(Scope{
                                    {}, currentScope, std::move(funcType)})});
      currentScope = symbolTable.at(stmt).get();
      typeCheckerTables.push(TypeCheckerTable{});
    }

    void exitScope()
    {
      currentScope = currentScope->parent;
      typeCheckerTables.pop();
    }

  public:
    SemanticVisitor(SymbolTable &symbolTable) : symbolTable(symbolTable) {}

    // nothing to check for types.
    void visit(IntTypeNode &node) override {}
    void visit(FloatTypeNode &node) override {}
    void visit(ColourTypeNode &node) override {}
    void visit(BoolTypeNode &node) override {}
    void visit(ArrayTypeNode &node) override {}

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
    void visit(NewArrExprNode &node) override;
    void visit(ArrayAccessNode &node) override;
    void visit(GetCharNode &node) override;
    void visit(Float2IntNode &node) override;

    void visit(AssignmentStmt &node) override;
    void visit(VariableDeclStmt &node) override;
    void visit(PrintStmt &node) override;
    void visit(DelayStmt &node) override;
    void visit(PixelStmt &node) override;
    void visit(PixelRStmt &node) override;
    void visit(ReturnStmt &node) override;
    void visit(PutCharStmt &node) override;
    void visit(IfElseStmt &node) override;
    void visit(ForStmt &node) override;
    void visit(WhileStmt &node) override;
    void visit(FuncDeclStmt &node) override;
    void visit(BlockStmt &node) override;

    void visit(TranslationUnit &node) override;
  };

} // namespace ast

#endif // SEMANTIC_VISITOR_H_
