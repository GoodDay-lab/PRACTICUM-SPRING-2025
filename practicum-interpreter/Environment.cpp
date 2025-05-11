#include "Environment.h"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <variant>

#include "DebugPrint.h"
#include "RuntimeError.h"

#define EXPECT_TRUE(x) __builtin_expect(static_cast<int64_t>(x), 1)
#define EXPECT_FALSE(x) __builtin_expect(static_cast<int64_t>(x), 0)

namespace cpplox::Evaluator {

namespace {
class UndefinedVarAccess : public std::exception {};
class UninitializedVarAccess : public std::exception {};
}  // namespace

// ================= //
// class Environment
// ================= //
Environment::Environment(EnvironmentPtr parentEnviron)
    : parentEnviron(std::move(parentEnviron)) {}

auto Environment::assign(size_t hashedVarName, LoxObject object) -> bool {
  auto iter = objects.find(hashedVarName);
  if (iter != objects.end()) {
    objects.insert_or_assign(hashedVarName, object);
    objects_T.insert_or_assign(hashedVarName, object.index());
    return true;
  }
  if (EXPECT_TRUE(parentEnviron != nullptr))
    return parentEnviron->assign(hashedVarName, std::move(object));

  throw UndefinedVarAccess();
}

void Environment::define(size_t hashedVarName, LoxObject object, size_t t) {
  objects.insert_or_assign(hashedVarName, object);
  objects_T.insert_or_assign(hashedVarName, t);
}

auto Environment::get(size_t hashedVarName) -> LoxObject {
  auto iter = objects.find(hashedVarName);
  if (EXPECT_TRUE(iter != objects.end())) {
    if (EXPECT_FALSE(std::holds_alternative<std::nullptr_t>(iter->second)))
      throw UninitializedVarAccess();
    return iter->second;
  }
  if (EXPECT_TRUE(parentEnviron != nullptr))
    return parentEnviron->get(hashedVarName);

  throw UndefinedVarAccess();  // throws only in the Global Environ
}

auto Environment::get_T(size_t hashedVarName) -> size_t {
  auto iter = objects.find(hashedVarName);
  if (EXPECT_TRUE(iter != objects.end())) {
    return objects_T.find(hashedVarName)->second;
  }
  if (EXPECT_TRUE(parentEnviron != nullptr))
    return parentEnviron->get_T(hashedVarName);

  throw UndefinedVarAccess();  // throws only in the Global Environ
}

auto Environment::isGlobal() -> bool { return (parentEnviron == nullptr); }

auto Environment::getParentEnv() -> EnvironmentPtr { return parentEnviron; }

// ======================== //
// class EnvironmentManager
// ======================== //
EnvironmentManager::EnvironmentManager(ErrorReporter& eReporter)
    : eReporter(eReporter),
      currEnviron(std::make_shared<Environment>(nullptr)) {
#ifdef ENVIRON_DEBUG
  ErrorsAndDebug::debugPrint(
      "EnvironmentManager is now alive! Global Envrion = "
      + std::to_string((uint64_t)currEnviron.get()));
#endif
}

void EnvironmentManager::createNewEnviron(const std::string& caller) {
  currEnviron = std::make_shared<Environment>(currEnviron);
#ifdef ENVIRON_DEBUG
  ErrorsAndDebug::debugPrint(caller + " requested new environ: "
                             + std::to_string((uint64_t)currEnviron.get())
                             + ".");
#endif
}

void EnvironmentManager::discardEnvironsTill(
    const Environment::EnvironmentPtr& environToRestore,
    const std::string& caller) {
#ifdef ENVIRON_DEBUG
  ErrorsAndDebug::debugPrint("discardEnvironsTill( "
                             + std::to_string((uint64_t)environToRestore.get())
                             + " ) called by " + caller + ". CurrEnviron: "
                             + std::to_string((uint64_t)currEnviron.get())
                             + ".");
#endif
  // Global environment should only be destroyed when the Environment Manager
  // goes away.
  while (EXPECT_TRUE(!currEnviron->isGlobal()
                     && currEnviron.get() != environToRestore.get())) {
#ifdef ENVIRON_DEBUG
    ErrorsAndDebug::debugPrint("Discarding environ "
                               + std::to_string((uint64_t)currEnviron.get())
                               + ".");
#endif
    currEnviron = currEnviron->getParentEnv();
  }
}

void EnvironmentManager::define(const std::string& tokenStr, LoxObject object, size_t t) {
  currEnviron->define(hasher(tokenStr), std::move(object), t);
}

void EnvironmentManager::define(const Types::Token& varToken,
                                LoxObject object, size_t t) {
  currEnviron->define(hasher(varToken.getLexeme()), std::move(object), t);
}

void EnvironmentManager::assign(const Types::Token& varToken,
                                LoxObject object) {
  try {
    currEnviron->assign(hasher(varToken.getLexeme()), std::move(object));
  } catch (const UndefinedVarAccess& e) {
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, varToken, "Can't assign to an undefined variable.");
  }
}

auto EnvironmentManager::get(const Types::Token& varToken) -> LoxObject {
  try {
    return currEnviron->get(hasher(varToken.getLexeme()));
  } catch (const UndefinedVarAccess& e) {
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, varToken, "Attempted to access an undefined variable.");
  } catch (const UninitializedVarAccess& e) {
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, varToken, "Attempted to access an uninitialized variable.");
  }
}

auto EnvironmentManager::get_T(const Types::Token& varToken) -> size_t {
  try {
    return currEnviron->get_T(hasher(varToken.getLexeme()));
  } catch (const UndefinedVarAccess& e) {
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, varToken, "Attempted to access an undefined variable.");
  } catch (const UninitializedVarAccess& e) {
    throw ErrorsAndDebug::reportRuntimeError(
        eReporter, varToken, "Attempted to access an uninitialized variable.");
  }
}

auto EnvironmentManager::getCurrEnv() -> Environment::EnvironmentPtr {
  return currEnviron;
}

void EnvironmentManager::setCurrEnv(Environment::EnvironmentPtr newCurr,
                                    const std::string& caller) {
#ifdef ENVIRON_DEBUG
  ErrorsAndDebug::debugPrint(
      caller + " requested setting currEnviron to: "
      + std::to_string((uint64_t)newCurr.get())
      + ". CurrEnviron: " + std::to_string((uint64_t)currEnviron.get()) + ".");
#endif
  currEnviron = std::move(newCurr);
}

}  // namespace cpplox::Evaluator
