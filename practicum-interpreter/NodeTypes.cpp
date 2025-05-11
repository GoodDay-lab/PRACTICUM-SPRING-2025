#include "NodeTypes.h"

#include <initializer_list>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace cpplox::AST {
// ========================== //
// Expr AST Type Constructors //
// ========================== //
BinaryExpr::BinaryExpr(ExprPtrVariant left, Token op, ExprPtrVariant right)
    : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

GroupingExpr::GroupingExpr(ExprPtrVariant expression)
    : expression(std::move(expression)) {}

LiteralExpr::LiteralExpr(OptionalLiteral value)
    : literalVal(std::move(value)) {}

UnaryExpr::UnaryExpr(Token op, ExprPtrVariant right)
    : op(std::move(op)), right(std::move(right)) {}

ConditionalExpr::ConditionalExpr(ExprPtrVariant condition,
                                 ExprPtrVariant thenBranch,
                                 ExprPtrVariant elseBranch)
    : condition(std::move(condition)),
      thenBranch(std::move(thenBranch)),
      elseBranch(std::move(elseBranch)) {}

VariableExpr::VariableExpr(Token varName) : varName(std::move(varName)) {}

AssignmentExpr::AssignmentExpr(Token varName, ExprPtrVariant right)
    : varName(std::move(varName)), right(std::move(right)) {}

LogicalExpr::LogicalExpr(ExprPtrVariant left, Token op, ExprPtrVariant right)
    : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}


// ==============================//
// EPV creation helper functions //
// ==============================//
auto createBinaryEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant {
  return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

auto createUnaryEPV(Token op, ExprPtrVariant right) -> ExprPtrVariant {
  return std::make_unique<UnaryExpr>(op, std::move(right));
}

auto createGroupingEPV(ExprPtrVariant right) -> ExprPtrVariant {
  return std::make_unique<GroupingExpr>(std::move(right));
}

auto createLiteralEPV(OptionalLiteral literal) -> ExprPtrVariant {
  return std::make_unique<LiteralExpr>(std::move(literal));
}

auto createConditionalEPV(ExprPtrVariant condition, ExprPtrVariant then,
                          ExprPtrVariant elseBranch) -> ExprPtrVariant {
  return std::make_unique<ConditionalExpr>(
      std::move(condition), std::move(then), std::move(elseBranch));
}

auto createVariableEPV(Token varName) -> ExprPtrVariant {
  return std::make_unique<VariableExpr>(varName);
}

auto createAssignmentEPV(Token varName, ExprPtrVariant expr) -> ExprPtrVariant {
  return std::make_unique<AssignmentExpr>(varName, std::move(expr));
}

auto createLogicalEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant {
  return std::make_unique<LogicalExpr>(std::move(left), op, std::move(right));
}

// =================== //
// Statment AST types; //
// =================== //
ExprStmt::ExprStmt(ExprPtrVariant expr) : expression(std::move(expr)) {}

// WriteStmt::WriteStmt(ExprPtrVariant expr) : expression(std::move(expr)) {}

WriteStmt::WriteStmt(std::vector<ExprPtrVariant> exprs) : expressions(std::move(exprs)) {}

ReadStmt::ReadStmt(Token name) : varName(name) {}

IntStmt::IntStmt(Token varName, std::optional<ExprPtrVariant> initializer)
    : varName(std::move(varName)), initializer(std::move(initializer)) {}

RealStmt::RealStmt(Token varName, std::optional<ExprPtrVariant> initializer)
    : varName(std::move(varName)), initializer(std::move(initializer)) {}

StrStmt::StrStmt(Token varName, std::optional<ExprPtrVariant> initializer)
    : varName(std::move(varName)), initializer(std::move(initializer)) {}

BlockStmt::BlockStmt(std::vector<StmtPtrVariant> statements)
    : statements(std::move(statements)) {}

IfStmt::IfStmt(ExprPtrVariant condition, StmtPtrVariant thenBranch,
               std::optional<StmtPtrVariant> elseBranch)
    : condition(std::move(condition)),
      thenBranch(std::move(thenBranch)),
      elseBranch(std::move(elseBranch)) {}

WhileStmt::WhileStmt(ExprPtrVariant condition, StmtPtrVariant loopBody)
    : condition(std::move(condition)), loopBody(std::move(loopBody)) {}

ForStmt::ForStmt(std::optional<StmtPtrVariant> initializer,
                 std::optional<ExprPtrVariant> condition,
                 std::optional<ExprPtrVariant> increment,
                 StmtPtrVariant loopBody)
    : initializer(std::move(initializer)),
      condition(std::move(condition)),
      increment(std::move(increment)),
      loopBody(std::move(loopBody)) {}

BreakStmt::BreakStmt(Token n) : name(n) {}

// ============================================================= //
// Helper functions to create StmtPtrVariants for each Stmt type //
// ============================================================= //
auto createExprSPV(ExprPtrVariant expr) -> StmtPtrVariant {
  return std::make_unique<ExprStmt>(std::move(expr));
}

/*
auto createWriteSPV(ExprPtrVariant expr) -> StmtPtrVariant {
  return std::make_unique<WriteStmt>(std::move(expr));
}
*/

auto createWriteSPV(std::vector<ExprPtrVariant> exprs) -> StmtPtrVariant {
  return std::make_unique<WriteStmt>(std::move(exprs));
}

auto createReadSPV(Token varName) -> StmtPtrVariant {
  return std::make_unique<ReadStmt>(varName);
}

auto createIntSPV(Token varName, std::optional<ExprPtrVariant> initializer)
    -> StmtPtrVariant {
  return std::make_unique<IntStmt>(varName, std::move(initializer));
}

auto createRealSPV(Token varName, std::optional<ExprPtrVariant> initializer)
    -> StmtPtrVariant {
  return std::make_unique<RealStmt>(varName, std::move(initializer));
}

auto createStrSPV(Token varName, std::optional<ExprPtrVariant> initializer)
    -> StmtPtrVariant {
  return std::make_unique<StrStmt>(varName, std::move(initializer));
}

auto createBlockSPV(std::vector<StmtPtrVariant> statements) -> StmtPtrVariant {
  return std::make_unique<BlockStmt>(std::move(statements));
}

auto createIfSPV(ExprPtrVariant condition, StmtPtrVariant thenBranch,
                 std::optional<StmtPtrVariant> elseBranch) -> StmtPtrVariant {
  return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch),
                                  std::move(elseBranch));
}

auto createWhileSPV(ExprPtrVariant condition, StmtPtrVariant loopBody)
    -> StmtPtrVariant {
  return std::make_unique<WhileStmt>(std::move(condition), std::move(loopBody));
}

auto createForSPV(std::optional<StmtPtrVariant> initializer,
                  std::optional<ExprPtrVariant> condition,
                  std::optional<ExprPtrVariant> increment,
                  StmtPtrVariant loopBody) -> StmtPtrVariant {
  return std::make_unique<ForStmt>(std::move(initializer), std::move(condition),
                                   std::move(increment), std::move(loopBody));
}

auto createBreakSPV(Token name) -> StmtPtrVariant {
  return std::make_unique<BreakStmt>(name);
}

}  // namespace cpplox::AST
