#include "Evaluator.h"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "PrettyPrinter.h"
#include "DebugPrint.h"
#include "RuntimeError.h"
#include "Objects.h"
#include "Literal.h"
#include "Token.h"
#include "Environment.h"

#define EXPECT_TRUE(x) __builtin_expect(static_cast<int64_t>(x), 1)
#define EXPECT_FALSE(x) __builtin_expect(static_cast<int64_t>(x), 0)

namespace cpplox::Evaluator {

// throws RuntimeError if right isn't a double
auto Evaluator::getDouble(const Token& token, const LoxObject& right)
    -> double {
  if (EXPECT_FALSE(!std::holds_alternative<double>(right)))
    throw reportRuntimeError(
        eReporter, token,
        "Attempted to perform arithmetic operation on non-numeric literal "
            + getObjectString(right));
  return std::get<double>(right);
}


//===============================//
// Expression Evaluation Methods //
//===============================//
auto Evaluator::evaluateBinaryExpr(const BinaryExprPtr& expr) -> LoxObject {
  auto left = evaluateExpr(expr->left);
  auto right = evaluateExpr(expr->right);
  switch (expr->op.getType()) {
    case TokenType::COMMA: return right;
    case TokenType::BANG_EQUAL: return !areEqual(left, right);
    case TokenType::EQUAL_EQUAL: return areEqual(left, right);
    case TokenType::MINUS:
      return getDouble(expr->op, left) - getDouble(expr->op, right);
    case TokenType::SLASH: {
      double denominator = getDouble(expr->op, right);
      if (EXPECT_FALSE(denominator == 0.0))
        throw reportRuntimeError(eReporter, expr->op,
                                 "Division by zero is illegal");
      return getDouble(expr->op, left) / denominator;
    }
    case TokenType::STAR:
      return getDouble(expr->op, left) * getDouble(expr->op, right);
    case TokenType::MOD:
      return (double)((int)getDouble(expr->op, left) % (int)getDouble(expr->op, right));
    case TokenType::LESS:
      return getDouble(expr->op, left) < getDouble(expr->op, right);
    case TokenType::LESS_EQUAL:
      return getDouble(expr->op, left) <= getDouble(expr->op, right);
    case TokenType::GREATER:
      return getDouble(expr->op, left) > getDouble(expr->op, right);
    case TokenType::GREATER_EQUAL:
      return getDouble(expr->op, left) >= getDouble(expr->op, right);
    case TokenType::PLUS: {
      if (std::holds_alternative<double>(left)
          && std::holds_alternative<double>(right)) {
        return std::get<double>(left) + std::get<double>(right);
      }
      if (std::holds_alternative<std::string>(left)
          || std::holds_alternative<std::string>(right)) {
        return getObjectString(left) + getObjectString(right);
      }
      throw reportRuntimeError(
          eReporter, expr->op,
          "Operands to 'plus' must be numbers or strings; This is invalid: "
              + getObjectString(left) + " + " + getObjectString(right));
    }
    default:
      throw reportRuntimeError(
          eReporter, expr->op,
          "Attempted to apply invalid operator to binary expr: "
              + expr->op.getTypeString());
  }
}

auto Evaluator::evaluateGroupingExpr(const GroupingExprPtr& expr) -> LoxObject {
  return evaluateExpr(expr->expression);
}

namespace {
auto getLoxObjectfromStringLiteral(const Literal& strLiteral) -> LoxObject {
  const auto& str = std::get<std::string>(strLiteral);
  if (str == "true") return LoxObject(true);
  if (str == "false") return LoxObject(false);
  if (str == "nil") return LoxObject(nullptr);
  return LoxObject(str);
};
}  // namespace

auto Evaluator::evaluateLiteralExpr(const LiteralExprPtr& expr) -> LoxObject {
  return expr->literalVal.has_value()
             ? std::holds_alternative<std::string>(expr->literalVal.value())
                   ? getLoxObjectfromStringLiteral(expr->literalVal.value())
                   : LoxObject(std::get<double>(expr->literalVal.value()))
             : LoxObject(nullptr);
}

auto Evaluator::evaluateUnaryExpr(const UnaryExprPtr& expr) -> LoxObject {
  LoxObject right = evaluateExpr(expr->right);
  switch (expr->op.getType()) {
    case TokenType::BANG: return !isTrue(right);
    case TokenType::MINUS: return -getDouble(expr->op, right);
    case TokenType::PLUS_PLUS: return getDouble(expr->op, right) + 1;
    case TokenType::MINUS_MINUS: return getDouble(expr->op, right) - 1;
    default:
      throw reportRuntimeError(
          eReporter, expr->op,
          "Illegal unary expression: " + expr->op.getLexeme()
              + getObjectString(right));
  }
}

auto Evaluator::evaluateConditionalExpr(const ConditionalExprPtr& expr)
    -> LoxObject {
  if (isTrue(evaluateExpr(expr->condition)))
    return evaluateExpr(expr->thenBranch);
  return evaluateExpr(expr->elseBranch);
}

auto Evaluator::evaluateVariableExpr(const VariableExprPtr& expr) -> LoxObject {
  return environManager.get(expr->varName);
}

auto Evaluator::evaluateAssignmentExpr(const AssignmentExprPtr& expr)
    -> LoxObject {
  environManager.assign(expr->varName, evaluateExpr(expr->right));
  return environManager.get(expr->varName);
}

auto Evaluator::evaluateLogicalExpr(const LogicalExprPtr& expr) -> LoxObject {
  LoxObject leftVal = evaluateExpr(expr->left);
  if (expr->op.getType() == TokenType::OR)
    return isTrue(leftVal) ? leftVal : evaluateExpr(expr->right);
  if (expr->op.getType() == TokenType::AND)
    return !isTrue(leftVal) ? leftVal : evaluateExpr(expr->right);

  throw reportRuntimeError(eReporter, expr->op,
                           "Illegal logical operator: " + expr->op.getLexeme());
}

auto Evaluator::evaluateExpr(const ExprPtrVariant& expr) -> LoxObject {
  switch (expr.index()) {
    case 0:  // BinaryExprPtr
      return evaluateBinaryExpr(std::get<0>(expr));
    case 1:  // GroupingExprPtr
      return evaluateGroupingExpr(std::get<1>(expr));
    case 2:  // LiteralExprPtr
      return evaluateLiteralExpr(std::get<2>(expr));
    case 3:  // UnaryExprPtr
      return evaluateUnaryExpr(std::get<3>(expr));
    case 4:  // ConditionalExprPtr
      return evaluateConditionalExpr(std::get<4>(expr));
    case 5:  // VariableExprPtr
      return evaluateVariableExpr(std::get<5>(expr));
    case 6:  // AssignmentExprPtr
      return evaluateAssignmentExpr(std::get<6>(expr));
    case 7:  // LogicalExprPtr
      return evaluateLogicalExpr(std::get<7>(expr));
    default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 8,
                    "Looks like you forgot to update the cases in "
                    "Evaluator::Evaluate(const ExptrVariant&)!");
      return "";
  }
}

//==============================//
// Statement Evaluation Methods //
//==============================//
auto Evaluator::evaluateExprStmt(const ExprStmtPtr& stmt)
    -> std::optional<LoxObject> {
#ifdef EVAL_DEBUG
  ErrorsAndDebug::debugPrint("evaluateExprStmt called.");
#endif  // EVAL_DEBUG

  LoxObject result = evaluateExpr(stmt->expression);

#ifdef EVAL_DEBUG
  ErrorsAndDebug::debugPrint("evaluateExprStmt: expression evaluation result: "
                             + getObjectString(result));
#endif  // EVAL_DEBUG
  return std::nullopt;
}

auto Evaluator::evaluateWriteStmt(const WriteStmtPtr& stmt)
    -> std::optional<LoxObject> {
#ifdef EVAL_DEBUG
  ErrorsAndDebug::debugPrint("evaluateWriteStmt called.");
#endif  // EVAL_DEBUG

  for (const auto &expr : stmt->expressions) {
    LoxObject objectToPrint = evaluateExpr(expr);
    std::cout << getObjectString(objectToPrint) << " ";
  } std::cout << std::endl;

#ifdef EVAL_DEBUG
  ErrorsAndDebug::debugPrint("evaluateWriteStmt should have printed."
                             + getObjectString(objectToPrint));
#endif  // EVAL_DEBUG
  return std::nullopt;
}

auto Evaluator::evaluateReadStmt(const ReadStmtPtr& stmt)
    -> std::optional<LoxObject> {
  try {
    auto _id = environManager.get_T(stmt->varName);
    if (_id == 0) {
        std::string input;
        std::cin >> input;
        environManager.assign(stmt->varName, input);
    } else if (_id == 1) {
        double input;
        std::cin >> input;
        environManager.assign(stmt->varName, input);
    } else if (_id == 2) {
        std::string input;
        std::cin >> input;
        environManager.assign(stmt->varName, (input == "true") 
                                             ? (true) 
                                             : ((input == "false")
                                               ? (false)
                                               : throw reportRuntimeError(eReporter,
                                                 stmt->varName, "Expected 'true' or 'false'")));
    }
  } catch(...) {
    std::string input;
    std::cin >> input;
    environManager.assign(stmt->varName, input);
  }

  return std::nullopt;
}

auto Evaluator::evaluateBlockStmt(const BlockStmtPtr& stmt)
    -> std::optional<LoxObject> {
  auto currEnviron = environManager.getCurrEnv();
  environManager.createNewEnviron();
  std::optional<LoxObject> result = evaluateStmts(stmt->statements);
  environManager.discardEnvironsTill(currEnviron);
  return result;
}

auto Evaluator::evaluateIntStmt(const IntStmtPtr& stmt)
    -> std::optional<LoxObject> {
  if (stmt->initializer.has_value()) {
    environManager.define(stmt->varName,
                          evaluateExpr(stmt->initializer.value()), 1);
  } else {
    environManager.define(stmt->varName, LoxObject(nullptr), 1);
  }
  return std::nullopt;
}

auto Evaluator::evaluateRealStmt(const RealStmtPtr& stmt)
    -> std::optional<LoxObject> {
  if (stmt->initializer.has_value()) {
    environManager.define(stmt->varName,
                          evaluateExpr(stmt->initializer.value()), 1);
  } else {
    environManager.define(stmt->varName, LoxObject(nullptr), 1);
  }
  return std::nullopt;
}

auto Evaluator::evaluateStrStmt(const StrStmtPtr& stmt)
    -> std::optional<LoxObject> {
  if (stmt->initializer.has_value()) {
    environManager.define(stmt->varName,
                          evaluateExpr(stmt->initializer.value()), 0);
  } else {
    environManager.define(stmt->varName, LoxObject(nullptr), 0);
  }
  return std::nullopt;
}

auto Evaluator::evaluateIfStmt(const IfStmtPtr& stmt)
    -> std::optional<LoxObject> {
  if (isTrue(evaluateExpr(stmt->condition)))
    return evaluateStmt(stmt->thenBranch);
  if (stmt->elseBranch.has_value())
    return evaluateStmt(stmt->elseBranch.value());
  return std::nullopt;
}

auto Evaluator::evaluateWhileStmt(const WhileStmtPtr& stmt)
    -> std::optional<LoxObject> {
  std::optional<LoxObject> result = std::nullopt;
  while (isTrue(evaluateExpr(stmt->condition)) && !result.has_value()) {
    try {
      result = evaluateStmt(stmt->loopBody);
    } catch(const BreakException& e) {
      return std::nullopt;
    }
  }
  return result;
}

auto Evaluator::evaluateForStmt(const ForStmtPtr& stmt)
    -> std::optional<LoxObject> {
  std::optional<LoxObject> result = std::nullopt;
  if (stmt->initializer.has_value()) evaluateStmt(stmt->initializer.value());
  while (true) {
    if (stmt->condition.has_value()
        && !isTrue(evaluateExpr(stmt->condition.value())))
      break;
    try {
      result = evaluateStmt(stmt->loopBody);
    } catch(const BreakException& e) {
      return std::nullopt;
    }
    if (result.has_value()) 
      break;
    if (stmt->increment.has_value()) evaluateExpr(stmt->increment.value());
  }
  return result;
}

auto Evaluator::evaluateBreakStmt(const BreakStmtPtr& stmt) -> std::optional<LoxObject> {
  throw BreakException(stmt->name.getLine());
}

auto Evaluator::evaluateStmt(const AST::StmtPtrVariant& stmt)
    -> std::optional<LoxObject> {
  switch (stmt.index()) {
    case 0:  // ExprStmtPtr
      return evaluateExprStmt(std::get<0>(stmt));
    case 1:  // PrintStmtPtr
      return evaluateWriteStmt(std::get<1>(stmt));
    case 2:  // PrintStmtPtr
      return evaluateReadStmt(std::get<2>(stmt));
    case 3:  // BlockStmtPtr
      return evaluateBlockStmt(std::get<3>(stmt));
    case 4:
      return evaluateIntStmt(std::get<4>(stmt));
    case 5:
      return evaluateRealStmt(std::get<5>(stmt));
    case 6:
      return evaluateStrStmt(std::get<6>(stmt));
    case 7:  // IfStmtPtr
      return evaluateIfStmt(std::get<7>(stmt));
    case 8:  // WhileStmtPtr
      return evaluateWhileStmt(std::get<8>(stmt));
    case 9:  // ForStmtPtr
      return evaluateForStmt(std::get<9>(stmt));
    case 10: // BreakStmtPtr
      return evaluateBreakStmt(std::get<10>(stmt));
    default:
      static_assert(
          std::variant_size_v<StmtPtrVariant> == 11,
          "Looks like you forgot to update the cases in "
          "PrettyPrinter::toString(const StmtPtrVariant& statement)!");
      return std::nullopt;
  }
}

auto Evaluator::evaluateStmts(const std::vector<AST::StmtPtrVariant>& stmts)
    -> std::optional<LoxObject> {
  std::optional<LoxObject> result = std::nullopt;
  for (const AST::StmtPtrVariant& stmt : stmts) {
    try {
      result = evaluateStmt(stmt);
      if (result.has_value())
        break;
    } catch (const ErrorsAndDebug::RuntimeError& e) {
      ErrorsAndDebug::debugPrint("Caught unhandled exception.");
      if (EXPECT_FALSE(++numRunTimeErr > MAX_RUNTIME_ERR)) {
        std::cerr << "Too many errors occurred. Exiting evaluation."
                  << std::endl;
        throw e;
      }
    }
  }
  return result;
}

Evaluator::Evaluator(ErrorReporter& eReporter)
    : eReporter(eReporter), environManager(eReporter) {} 

}  // namespace cpplox::Evaluator
