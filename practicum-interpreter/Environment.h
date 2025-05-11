#ifndef CPPLOX_EVALUATOR_ENVIRONMENT__H
#define CPPLOX_EVALUATOR_ENVIRONMENT__H
#pragma once

#include <cstddef>
#include <exception>
#include <list>
#include <map>
#include <memory>
#include <optional>

#include "ErrorReporter.h"
#include "Objects.h"
#include "Token.h"
#include "Uncopyable.h"

namespace cpplox::Evaluator {
using ErrorsAndDebug::ErrorReporter;

// Do not use the Environment directly. Use the EnvironmentManager class below
// instead to manage it.
class Environment : public Types::Uncopyable,
                    std::enable_shared_from_this<Environment> {
 public:
  using EnvironmentPtr = std::shared_ptr<Environment>;

  explicit Environment(EnvironmentPtr parentEnviron);

  auto assign(size_t hashedVarName, LoxObject object) -> bool;
  void define(size_t hashedVarName, LoxObject object, size_t t = 3);
  auto get(size_t hashedVarName) -> LoxObject;
  auto get_T(size_t hashedVarName) -> size_t;
  auto getParentEnv() -> EnvironmentPtr;
  auto isGlobal() -> bool;

 private:
  std::map<size_t, LoxObject> objects;
  std::map<size_t, size_t> objects_T;
  EnvironmentPtr parentEnviron = nullptr;
};

class EnvironmentManager : public Types::Uncopyable {
 public:
  explicit EnvironmentManager(ErrorReporter& eReporter);

  void assign(const Types::Token& varToken, LoxObject object);
  void createNewEnviron(const std::string& caller = __builtin_FUNCTION());
  void discardEnvironsTill(const Environment::EnvironmentPtr& environToRestore,
                           const std::string& caller = __builtin_FUNCTION());
  void define(const std::string& tokenStr, LoxObject object, size_t t = 3);
  void define(const Types::Token& varToken, LoxObject object, size_t t = 3);
  auto get(const Types::Token& varToken) -> LoxObject;
  auto get(const std::string& varToken) -> LoxObject;
  auto get_T(const Types::Token& varToken) -> size_t;
  auto get_T(const std::string& varToken) -> size_t; 
  auto getCurrEnv() -> Environment::EnvironmentPtr;
  void setCurrEnv(Environment::EnvironmentPtr newCurr,
                  const std::string& caller = __builtin_FUNCTION());

 private:
  ErrorReporter& eReporter;
  Environment::EnvironmentPtr currEnviron;
  std::hash<std::string> hasher;
};

}  // namespace cpplox::Evaluator
#endif  // CPPLOX_EVALUATOR_ENVIRONMENT__H
