#ifndef CPPLOX_AST_NodeTypes_H
#define CPPLOX_AST_NodeTypes_H
#pragma once

// This header file describes AST node Types for both Expressions and Statements
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "Literal.h"
#include "Token.h"
#include "Uncopyable.h"

namespace cpplox::AST {
using Types::Literal;
using Types::OptionalLiteral;
using Types::Token;
using Types::TokenType;
using Types::Uncopyable;

// Forward declare all the Expression Types so we can define their pointers
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;
struct ConditionalExpr;
struct VariableExpr;
struct AssignmentExpr;
struct LogicalExpr;

// Unique_pointer sugar for Exprs.
using BinaryExprPtr = std::unique_ptr<BinaryExpr>;
using GroupingExprPtr = std::unique_ptr<GroupingExpr>;
using LiteralExprPtr = std::unique_ptr<LiteralExpr>;
using UnaryExprPtr = std::unique_ptr<UnaryExpr>;
using ConditionalExprPtr = std::unique_ptr<ConditionalExpr>;
using VariableExprPtr = std::unique_ptr<VariableExpr>;
using AssignmentExprPtr = std::unique_ptr<AssignmentExpr>;
using LogicalExprPtr = std::unique_ptr<LogicalExpr>;

// The variant that we will use to pass around pointers to each of these
// expression types. I'm exploring this so we don't have to rely on vTables
// for dynamic dispatch on visitor functions.
using ExprPtrVariant
    = std::variant<BinaryExprPtr, GroupingExprPtr, LiteralExprPtr, UnaryExprPtr,
                   ConditionalExprPtr, VariableExprPtr,
                   AssignmentExprPtr, LogicalExprPtr>;

// Forward Declaration of Statement Node types;
struct ExprStmt;
struct WriteStmt;
struct ReadStmt;
struct BlockStmt;
struct IntStmt;
struct RealStmt;
struct StrStmt;
struct IfStmt;
struct WhileStmt;
struct ForStmt;
struct BreakStmt;

// Unique pointer sugar for Stmts
using ExprStmtPtr = std::unique_ptr<ExprStmt>;
using WriteStmtPtr = std::unique_ptr<WriteStmt>;
using ReadStmtPtr = std::unique_ptr<ReadStmt>;
using BlockStmtPtr = std::unique_ptr<BlockStmt>;
using IntStmtPtr = std::unique_ptr<IntStmt>;
using RealStmtPtr = std::unique_ptr<RealStmt>;
using StrStmtPtr = std::unique_ptr<StrStmt>;
using IfStmtPtr = std::unique_ptr<IfStmt>;
using WhileStmtPtr = std::unique_ptr<WhileStmt>;
using ForStmtPtr = std::unique_ptr<ForStmt>;
using BreakStmtPtr = std::unique_ptr<BreakStmt>;

// We use this variant to pass around pointers to each of these Stmt types,
// without having to resort to virtual functions and dynamic dispatch
using StmtPtrVariant
    = std::variant<ExprStmtPtr, WriteStmtPtr, ReadStmtPtr, BlockStmtPtr, IntStmtPtr, RealStmtPtr,
                   StrStmtPtr, IfStmtPtr, WhileStmtPtr, ForStmtPtr, BreakStmtPtr>;

// Helper functions to create ExprPtrVariants for each Expr type
auto createBinaryEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant;
auto createUnaryEPV(Token op, ExprPtrVariant right) -> ExprPtrVariant;
auto createGroupingEPV(ExprPtrVariant right) -> ExprPtrVariant;
auto createLiteralEPV(OptionalLiteral literal) -> ExprPtrVariant;
auto createConditionalEPV(ExprPtrVariant condition, ExprPtrVariant then,
                          ExprPtrVariant elseBranch) -> ExprPtrVariant;
auto createVariableEPV(Token varName) -> ExprPtrVariant;
auto createAssignmentEPV(Token varName, ExprPtrVariant expr) -> ExprPtrVariant;
auto createLogicalEPV(ExprPtrVariant left, Token op, ExprPtrVariant right)
    -> ExprPtrVariant;

// Helper functions to create StmtPtrVariants for each Stmt type
auto createExprSPV(ExprPtrVariant expr) -> StmtPtrVariant;
//auto createWriteSPV(ExprPtrVariant expr) -> StmtPtrVariant;
auto createWriteSPV(std::vector<ExprPtrVariant> exprs) -> StmtPtrVariant;
auto createReadSPV(Token varName) -> StmtPtrVariant;
auto createBlockSPV(std::vector<StmtPtrVariant> statements) -> StmtPtrVariant;
auto createIntSPV(Token varName, std::optional<ExprPtrVariant> initializer)
    -> StmtPtrVariant;
auto createRealSPV(Token varName, std::optional<ExprPtrVariant> initializer)
    -> StmtPtrVariant;
auto createStrSPV(Token varName, std::optional<ExprPtrVariant> initializer)
    -> StmtPtrVariant;
auto createIfSPV(ExprPtrVariant condition, StmtPtrVariant thenBranch,
                 std::optional<StmtPtrVariant> elseBranch) -> StmtPtrVariant;
auto createWhileSPV(ExprPtrVariant condition, StmtPtrVariant loopBody)
    -> StmtPtrVariant;
auto createForSPV(std::optional<StmtPtrVariant> initializer,
                  std::optional<ExprPtrVariant> condition,
                  std::optional<ExprPtrVariant> increment,
                  StmtPtrVariant loopBody) -> StmtPtrVariant;
auto createBreakSPV(Token name) -> StmtPtrVariant;

// Expression AST Types:

struct BinaryExpr final : public Uncopyable {
  ExprPtrVariant left;
  Token op;
  ExprPtrVariant right;
  BinaryExpr(ExprPtrVariant left, Token op, ExprPtrVariant right);
};

struct GroupingExpr final : public Uncopyable {
  ExprPtrVariant expression;
  explicit GroupingExpr(ExprPtrVariant expression);
};

struct LiteralExpr final : public Uncopyable {
  OptionalLiteral literalVal;
  explicit LiteralExpr(OptionalLiteral value);
};

struct UnaryExpr final : public Uncopyable {
  Token op;
  ExprPtrVariant right;
  UnaryExpr(Token op, ExprPtrVariant right);
};

struct ConditionalExpr final : public Uncopyable {
  ExprPtrVariant condition;
  ExprPtrVariant thenBranch;
  ExprPtrVariant elseBranch;
  ConditionalExpr(ExprPtrVariant condition, ExprPtrVariant thenBranch,
                  ExprPtrVariant elseBranch);
};

struct VariableExpr final : public Uncopyable {
  Token varName;
  explicit VariableExpr(Token varName);
};

struct AssignmentExpr final : public Uncopyable {
  Token varName;
  ExprPtrVariant right;
  AssignmentExpr(Token varName, ExprPtrVariant right);
};

struct LogicalExpr final : public Uncopyable {
  ExprPtrVariant left;
  Token op;
  ExprPtrVariant right;
  LogicalExpr(ExprPtrVariant left, Token op, ExprPtrVariant right);
};


// Statment AST types;
struct ExprStmt final : public Uncopyable {
  ExprPtrVariant expression;
  explicit ExprStmt(ExprPtrVariant expr);
};

struct WriteStmt final : public Uncopyable {
  //ExprPtrVariant expression;
  std::vector<ExprPtrVariant> expressions;
  //explicit WriteStmt(ExprPtrVariant expression);
  explicit WriteStmt(std::vector<ExprPtrVariant> expressions);
};

struct ReadStmt final : public Uncopyable {
  Token varName;
  explicit ReadStmt(Token varName);
};

struct BlockStmt final : public Uncopyable {
  std::vector<StmtPtrVariant> statements;
  explicit BlockStmt(std::vector<StmtPtrVariant> statements);
};

struct IntStmt final : public Uncopyable {
  Token varName;
  std::optional<ExprPtrVariant> initializer;
  explicit IntStmt(Token varName, std::optional<ExprPtrVariant> initializer);
};

struct StrStmt final : public Uncopyable {
  Token varName;
  std::optional<ExprPtrVariant> initializer;
  explicit StrStmt(Token varName, std::optional<ExprPtrVariant> initializer);
};

struct RealStmt final : public Uncopyable {
  Token varName;
  std::optional<ExprPtrVariant> initializer;
  explicit RealStmt(Token varName, std::optional<ExprPtrVariant> initializer);
};

struct IfStmt final : public Uncopyable {
  ExprPtrVariant condition;
  StmtPtrVariant thenBranch;
  std::optional<StmtPtrVariant> elseBranch;
  explicit IfStmt(ExprPtrVariant condition, StmtPtrVariant thenBranch,
                  std::optional<StmtPtrVariant> elseBranch);
};

struct WhileStmt final : public Uncopyable {
  ExprPtrVariant condition;
  StmtPtrVariant loopBody;
  explicit WhileStmt(ExprPtrVariant condition, StmtPtrVariant loopBody);
};

struct ForStmt final : public Uncopyable {
  std::optional<StmtPtrVariant> initializer;
  std::optional<ExprPtrVariant> condition;
  std::optional<ExprPtrVariant> increment;
  StmtPtrVariant loopBody;
  explicit ForStmt(std::optional<StmtPtrVariant> initializer,
                   std::optional<ExprPtrVariant> condition,
                   std::optional<ExprPtrVariant> increment,
                   StmtPtrVariant loopBody);
};


struct BreakStmt : public Uncopyable {
  Token name;
  explicit BreakStmt(Token name);
};

}  // namespace cpplox::AST

#endif  // CPPLOX_AST_NodeTypes_H
