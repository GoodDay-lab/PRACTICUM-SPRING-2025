#ifndef CPPLOX_EVALUATOR_EVALUATOR__H
#define CPPLOX_EVALUATOR_EVALUATOR__H
#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "NodeTypes.h"
#include "ErrorReporter.h"
#include "Environment.h"
#include "Objects.h"
#include "Token.h"
#include "Uncopyable.h"

namespace cpplox::Evaluator {
using AST::AssignmentExprPtr;
using AST::BinaryExprPtr;
using AST::ConditionalExprPtr;
using AST::ExprPtrVariant;
using AST::GroupingExprPtr;
using AST::LiteralExprPtr;
using AST::LogicalExprPtr;
using AST::UnaryExprPtr;
using AST::VariableExprPtr;

using AST::BlockStmtPtr;
using AST::BreakStmtPtr;
using AST::ExprStmtPtr;
using AST::ForStmtPtr;
using AST::IfStmtPtr;
using AST::WriteStmtPtr;
using AST::ReadStmtPtr;
using AST::StmtPtrVariant;
using AST::IntStmtPtr;
using AST::StrStmtPtr;
using AST::RealStmtPtr;
using AST::WhileStmtPtr;

using ErrorsAndDebug::ErrorReporter;

using Types::Literal;
using Types::Token;
using Types::TokenType;

using ErrorsAndDebug::ErrorReporter;

class Evaluator {
 public:
  explicit Evaluator(ErrorReporter& eReporter);
  auto evaluateExpr(const ExprPtrVariant& expr) -> LoxObject;
  auto evaluateStmt(const AST::StmtPtrVariant& stmt)
      -> std::optional<LoxObject>;
  auto evaluateStmts(const std::vector<AST::StmtPtrVariant>& stmts)
      -> std::optional<LoxObject>;

 private:
  // evaluation functions for Expr types
  auto evaluateBinaryExpr(const BinaryExprPtr& expr) -> LoxObject;
  auto evaluateGroupingExpr(const GroupingExprPtr& expr) -> LoxObject;
  static auto evaluateLiteralExpr(const LiteralExprPtr& expr) -> LoxObject;
  auto evaluateUnaryExpr(const UnaryExprPtr& expr) -> LoxObject;
  auto evaluateConditionalExpr(const ConditionalExprPtr& expr) -> LoxObject;
  auto evaluateVariableExpr(const VariableExprPtr& expr) -> LoxObject;
  auto evaluateAssignmentExpr(const AssignmentExprPtr& expr) -> LoxObject;
  auto evaluateLogicalExpr(const LogicalExprPtr& expr) -> LoxObject;
  
  // evaluation functions for Stmt types
  auto evaluateExprStmt(const ExprStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateWriteStmt(const WriteStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateReadStmt(const ReadStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateBlockStmt(const BlockStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateIntStmt(const IntStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateStrStmt(const StrStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateRealStmt(const RealStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateIfStmt(const IfStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateWhileStmt(const WhileStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateForStmt(const ForStmtPtr& stmt) -> std::optional<LoxObject>;
  auto evaluateBreakStmt(const BreakStmtPtr& stmt) -> std::optional<LoxObject>;

  // throws RuntimeError if right isn't a double
  auto getDouble(const Token& token, const LoxObject& right) -> double;

  ErrorReporter& eReporter;
  EnvironmentManager environManager;

  static const int MAX_RUNTIME_ERR = 20;
  int numRunTimeErr = 0;
};

}  // namespace cpplox::Evaluator
#endif  // CPPLOX_EVALUATOR_EVALUATOR__H
