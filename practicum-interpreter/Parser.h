#ifndef CPPLOX_PARSER_PARSER_H
#define CPPLOX_PARSER_PARSER_H
#pragma once

#include <exception>
#include <functional>
#include <iterator>
#include <vector>

#include "NodeTypes.h"
#include "ErrorReporter.h"
#include "Token.h"

// This is a recursive descent parser for the lox language.
// Currently it only parses expressions.

// clang-format off
// Grammar production rules:
// program      -> program { <descriptions> <operators> }
// descriptions -> [ <description>; ]*
// description  -> [ <type> <variable> [, <variable>]*;
// type         -> int | string | real
// variable     -> <identifier> | <identifier> = <const>
// const        -> <integer> | <string> | <real>
// integer      -> [<sign>] [<digit>]+
// sign         -> + | -
// string       -> "{<literal>}"
// real         -> [<sign>] [<digit>]+.[<digit>]+
// operators    -> { <operator> }
// operator     -> if (<expression>) <operator> else <operator> |
//                  while (<expression>) <operator> |
//                  read (<identifier>); |
//                  write (<expression>) [, <expression>]*); |
//                  for ([<expression>]; [<expression>]; [<expression>]) <operator> |
//                  <complexexpr> | <exproperator> | break; 
// complexexpr  -> { <operators> }
// exproperator -> <expression>;
//
// clang-format on

namespace cpplox::Parser {
using AST::ExprPtrVariant;
using AST::StmtPtrVariant;

class RDParser {
 public:
  explicit RDParser(const std::vector<Types::Token>& tokens,
                    ErrorsAndDebug::ErrorReporter& eReporter);

  class RDParseError : std::exception {};  // Exception types

  // The public method to kick off parsing.
  // Stands in for "program" in the grammar
  // Currently it's mainly used to catch any exceptions that may be produced
  // (e.g., RDParseError) and deal with them.
  auto parse() -> std::vector<StmtPtrVariant>;

 private:
  // Grammar parsing functions
  // Statment parsing
  void program();
  auto declaration() -> std::optional<StmtPtrVariant>;
  auto intDecl() -> void;
  auto strDecl() -> void;
  auto realDecl() -> void;
  auto statement() -> StmtPtrVariant;
  auto exprStmt() -> StmtPtrVariant;
  auto readStmt() -> StmtPtrVariant;
  auto writeStmt() -> StmtPtrVariant;
  auto blockStmt() -> StmtPtrVariant;
  auto ifStmt() -> StmtPtrVariant;
  auto whileStmt() -> StmtPtrVariant;
  auto forStmt() -> StmtPtrVariant;
  auto breakStmt() -> StmtPtrVariant;

  // Expression Parsing
  auto expression() -> ExprPtrVariant;
  auto comma() -> ExprPtrVariant;
  auto assignment() -> ExprPtrVariant;
  auto conditional() -> ExprPtrVariant;
  auto logical_or() -> ExprPtrVariant;
  auto logical_and() -> ExprPtrVariant;
  auto equality() -> ExprPtrVariant;
  auto comparison() -> ExprPtrVariant;
  auto addition() -> ExprPtrVariant;
  auto multiplication() -> ExprPtrVariant;
  auto unary() -> ExprPtrVariant;
  auto primary() -> ExprPtrVariant;

  // Helper functions to implement the parser
  void advance();
  void consumeOrError(Types::TokenType tType, const std::string& errorMessage);
  using parserFn = ExprPtrVariant (RDParser::*)();
  auto consumeAnyBinaryExprs(
      const std::initializer_list<Types::TokenType>& types, ExprPtrVariant expr,
      const parserFn& f) -> ExprPtrVariant;
  auto consumeOneLiteral() -> ExprPtrVariant;
  auto consumeOneLiteral(const std::string& str) -> ExprPtrVariant;
  auto consumeGroupingExpr() -> ExprPtrVariant;
  void consumeSemicolonOrError();
  auto consumeSuper() -> ExprPtrVariant;
  auto consumeUnaryExpr() -> ExprPtrVariant;
  auto consumeVarExpr() -> ExprPtrVariant;
  auto error(const std::string& eMessage) -> RDParseError;
  [[nodiscard]] auto getCurrentTokenType() const -> Types::TokenType;
  auto getTokenAndAdvance() -> Types::Token;
  [[nodiscard]] auto isAtEnd() const -> bool;
  [[nodiscard]] auto match(
      const std::initializer_list<Types::TokenType>& types) const -> bool;
  [[nodiscard]] auto match(Types::TokenType type) const -> bool;
  [[nodiscard]] auto matchNext(Types::TokenType type) -> bool;
  [[nodiscard]] auto peek() const -> Types::Token;
  void reportError(const std::string& message);
  void synchronize();
  void throwOnErrorProduction(
      const std::initializer_list<Types::TokenType>& types, const parserFn& f);
  void throwOnErrorProductions();

  // The data the parser operates on.
  const std::vector<Types::Token>& tokens;
  std::vector<cpplox::Types::Token>::const_iterator currentIter;
  // std::__wrap_iter<std::vector<cpplox::Types::Token>::const_pointer>
  // currentIter;
  ErrorsAndDebug::ErrorReporter& eReporter;
  std::vector<StmtPtrVariant> statements;

  static const int MAX_ARGS = 255;

};  // class RDParser

}  // namespace cpplox::Parser
#endif  // CPPLOX_PARSER_PARSER_H
