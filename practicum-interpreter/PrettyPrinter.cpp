#include "PrettyPrinter.h"

#include <iterator>
#include <utility>
#include <vector>

#include "NodeTypes.h"
#include "Literal.h"

namespace cpplox::AST {
//=========================//
// Expr Printing functions //
//=========================//
namespace {
auto parenthesize(const std::string& name, const ExprPtrVariant& expr)
    -> std::string {
  return "(" + name + " " + PrettyPrinter::toString(expr) + ")";
}

auto parenthesize(const std::string& name, const ExprPtrVariant& expr1,
                  const ExprPtrVariant& expr2) -> std::string {
  return "(" + name + " " + PrettyPrinter::toString(expr1) + " "
         + PrettyPrinter::toString(expr2) + ")";
}

auto parenthesize(const std::string& name, const std::vector<ExprPtrVariant>& exprs)
    -> std::string {
  std::string retValue = "(" + name + " ";
  for (const auto& e : exprs)
    retValue += PrettyPrinter::toString(e);
  return retValue + ")";
}

auto printBinaryExpr(const BinaryExprPtr& expr) -> std::string {
  return parenthesize(expr->op.getLexeme(), expr->left, expr->right);
}
auto printGroupingExpr(const GroupingExprPtr& expr) -> std::string {
  std::string name = "group";
  return parenthesize(name, expr->expression);
}

auto printLiteralExpr(const LiteralExprPtr& expr) -> std::string {
  return expr->literalVal.has_value()
             ? Types::getLiteralString(expr->literalVal.value())
             : "nil";
}

auto printUnaryExpr(const UnaryExprPtr& expr) -> std::string {
  return parenthesize(expr->op.getLexeme(), expr->right);
}

auto printConditionalExpr(const ConditionalExprPtr& expr) -> std::string {
  return parenthesize(": " + parenthesize("?", expr->condition),
                      expr->thenBranch, expr->elseBranch);
}

auto printVariableExpr(const VariableExprPtr& expr) -> std::string {
  return "(" + expr->varName.getLexeme() + ")";
}

auto printAssignmentExpr(const AssignmentExprPtr& expr) -> std::string {
  return parenthesize("= " + expr->varName.getLexeme(), expr->right) + ";";
}

auto printLogicalExpr(const LogicalExprPtr& expr) -> std::string {
  return parenthesize(expr->op.getLexeme(), expr->left, expr->right);
}
// myGloriousFn(arg1, expr1+expr2)
// ( ((arg1), (+ expr1 expr2)) myGloriousFn )
}  // namespace

auto PrettyPrinter::toString(const ExprPtrVariant& expression) -> std::string {
  switch (expression.index()) {
    case 0:  // BinaryExprPtr
      return printBinaryExpr(std::get<0>(expression));
    case 1:  // GroupingExprPtr
      return printGroupingExpr(std::get<1>(expression));
    case 2:  // LiteralExprPtr
      return printLiteralExpr(std::get<2>(expression));
    case 3:  // UnaryExprPtr
      return printUnaryExpr(std::get<3>(expression));
    case 4:  // ConditionalExprPtr
      return printConditionalExpr(std::get<4>(expression));
    case 5:  // VariableExprPtr
      return printVariableExpr(std::get<5>(expression));
    case 6:  // AssignmentExprPtr
      return printAssignmentExpr(std::get<6>(expression));
    case 7:  // LogicalExprPtr
      return printLogicalExpr(std::get<7>(expression));
   default:
      static_assert(std::variant_size_v<ExprPtrVariant> == 8,
                    "Looks like you forgot to update the cases in "
                    "PrettyPrinter::toString(const ExptrVariant&)!");
      return "";
  }
}

namespace {

auto printExprStmt(const ExprStmtPtr& stmt) -> std::string {
  return parenthesize("", stmt->expression) + ";";
}

auto printWriteStmt(const WriteStmtPtr& stmt) -> std::string {
  return parenthesize("write(", stmt->expressions) + ");";
}

auto printReadStmt(const ReadStmtPtr& stmt) -> std::string {
  return "read(" + stmt->varName.getLexeme() + ");";
}

auto printBlockStmt(const BlockStmtPtr& blkStmts) -> std::vector<std::string> {
  std::vector<std::string> blkStmtStrsVec;
  blkStmtStrsVec.emplace_back("{");
  for (auto& stmt : blkStmts->statements) {
    auto stmtStr = PrettyPrinter::toString(stmt);
    if (!stmtStr.empty())
      std::move(stmtStr.begin(), stmtStr.end(),
                std::back_inserter(blkStmtStrsVec));
  }
  blkStmtStrsVec.emplace_back("}");
  return blkStmtStrsVec;
}

auto printIntStmt(const IntStmtPtr& stmt) -> std::string {
  std::string str = "var " + stmt->varName.getLexeme();
  if (stmt->initializer.has_value()) {
    str = "( = ( " + str + " ) "
          + PrettyPrinter::toString(stmt->initializer.value()) + " )";
  }
  return str + ";";
}

auto printRealStmt(const RealStmtPtr& stmt) -> std::string {
  std::string str = "var " + stmt->varName.getLexeme();
  if (stmt->initializer.has_value()) {
    str = "( = ( " + str + " ) "
          + PrettyPrinter::toString(stmt->initializer.value()) + " )";
  }
  return str + ";";
}

auto printStrStmt(const StrStmtPtr& stmt) -> std::string {
  std::string str = "var " + stmt->varName.getLexeme();
  if (stmt->initializer.has_value()) {
    str = "( = ( " + str + " ) "
          + PrettyPrinter::toString(stmt->initializer.value()) + " )";
  }
  return str + ";";
}

auto printIfStmt(const IfStmtPtr& stmt) -> std::vector<std::string> {
  std::vector<std::string> ifStmtStrVec;
  ifStmtStrVec.emplace_back("( if (" + PrettyPrinter::toString(stmt->condition)
                            + ")");
  auto thenBranchVec = PrettyPrinter::toString(stmt->thenBranch);
  std::move(thenBranchVec.begin(), thenBranchVec.end(),
            std::back_inserter(ifStmtStrVec));
  if (stmt->elseBranch.has_value()) {
    ifStmtStrVec.emplace_back(" else ");
    auto elseBranchVec = PrettyPrinter::toString(stmt->elseBranch.value());
    std::move(elseBranchVec.begin(), elseBranchVec.end(),
              std::back_inserter(ifStmtStrVec));
  }
  ifStmtStrVec.emplace_back(" );");
  return ifStmtStrVec;
}

auto printWhileStmt(const WhileStmtPtr& stmt) {
  std::vector<std::string> whileStmtStrVec;
  whileStmtStrVec.emplace_back(
      "( while (" + PrettyPrinter::toString(stmt->condition) + ")");
  auto loopBodyVec = PrettyPrinter::toString(stmt->loopBody);
  std::move(loopBodyVec.begin(), loopBodyVec.end(),
            std::back_inserter(whileStmtStrVec));
  whileStmtStrVec.emplace_back(" );");
  return whileStmtStrVec;
}

auto printForStmt(const ForStmtPtr& stmt) {
  std::vector<std::string> forStmtStrVec;
  forStmtStrVec.emplace_back("( for (");
  if (stmt->initializer.has_value()) {
    auto initVec = PrettyPrinter::toString(stmt->initializer.value());
    std::move(initVec.begin(), initVec.end(),
              std::back_inserter(forStmtStrVec));
  }
  forStmtStrVec.emplace_back(
      "; "
      + (stmt->condition.has_value()
             ? PrettyPrinter::toString(stmt->condition.value())
             : std::string())
      + ";"
      + (stmt->increment.has_value()
             ? PrettyPrinter::toString(stmt->increment.value())
             : std::string())
      + ")");

  auto loopBodyVec = PrettyPrinter::toString(stmt->loopBody);
  std::move(loopBodyVec.begin(), loopBodyVec.end(),
            std::back_inserter(forStmtStrVec));
  forStmtStrVec.emplace_back(" );");
  return forStmtStrVec;
}

auto printBreakStmt(const BreakStmtPtr& stmt) -> std::string {
  return "( break );";
}

}  // namespace

auto PrettyPrinter::toString(const StmtPtrVariant& statement)
    -> std::vector<std::string> {
  switch (statement.index()) {
    case 0:  // ExprStmtPtr
      return std::vector(1, printExprStmt(std::get<0>(statement)));
    case 1:  // PrintStmtPtr
      return std::vector(1, printWriteStmt(std::get<1>(statement)));
    case 2:  // PrintStmtPtr
      return std::vector(1, printReadStmt(std::get<2>(statement)));
    case 3:  // BlockStmtPtr
      return printBlockStmt(std::get<3>(statement));
    case 4:  // VarStmtPtr
      return std::vector(1, printIntStmt(std::get<4>(statement)));
    case 5:  // VarStmtPtr
      return std::vector(1, printRealStmt(std::get<5>(statement)));
    case 6:  // VarStmtPtr
      return std::vector(1, printStrStmt(std::get<6>(statement)));
    case 7:  // VarStmtPtr
      return printIfStmt(std::get<7>(statement));
    case 8:  // WhileStmtPtr
      return printWhileStmt(std::get<8>(statement));
    case 9:  // ForStmtPtr
      return printForStmt(std::get<9>(statement));
    case 10:
      return std::vector(1, printBreakStmt(std::get<10>(statement)));
    default:
      static_assert(
          std::variant_size_v<StmtPtrVariant> == 11,
          "Looks like you forgot to update the cases in "
          "PrettyPrinter::toString(const StmtPtrVariant& statement)!");
      return {};
  }
}

auto PrettyPrinter::toString(const std::vector<AST::StmtPtrVariant>& statements)
    -> std::vector<std::string> {
  std::vector<std::string> stmtStrsVec;
  for (const auto& stmt : statements) {
    auto stmtStr = PrettyPrinter::toString(stmt);
    if (!stmtStr.empty())
      std::move(stmtStr.begin(), stmtStr.end(),
                std::back_inserter(stmtStrsVec));
  }
  return stmtStrsVec;
}

}  // namespace cpplox::AST
