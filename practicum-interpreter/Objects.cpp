#include "Objects.h"

#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include "RuntimeError.h"

namespace cpplox::Evaluator {

// LoxObject Functions
auto areEqual(const LoxObject& left, const LoxObject& right) -> bool {
  if (left.index() == right.index()) {
    switch (left.index()) {
      case 0:  // string
        return std::get<std::string>(left) == std::get<std::string>(right);
      case 1:  // double
        return std::get<double>(left) == std::get<double>(right);
      case 2:  // bool
        return std::get<bool>(left) == std::get<bool>(right);
      case 3:  // std::nullptr_t
        // The case where one is null and the other isn't is handled by the
        // outer condition;
        return true;
      default:
        static_assert(std::variant_size_v<LoxObject> == 4,
                      "Looks like you forgot to update the cases in "
                      "ExprEvaluator::areEqual(const LoxObject&, const "
                      "LoxObject&)!");
    }
  }
  return false;
}

auto getObjectString(const LoxObject& object) -> std::string {
  switch (object.index()) {
    case 0:  // string
      return std::get<0>(object);
    case 1: {  // double
      std::string result = std::to_string(std::get<1>(object));
      auto pos = result.find(".000000");
      if (pos != std::string::npos)
        result.erase(pos, std::string::npos);
      else
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
      return result;
    }
    case 2:  // bool
      return std::get<2>(object) == true ? "true" : "false";
    case 3:  // nullptr
      return "nil";
    default:
      static_assert(std::variant_size_v<LoxObject> == 4,
                    "Looks like you forgot to update the cases in "
                    "getLiteralString()!");
      return "";
  }
}

auto isTrue(const LoxObject& object) -> bool {
  if (std::holds_alternative<std::nullptr_t>(object)) return false;
  if (std::holds_alternative<bool>(object)) return std::get<bool>(object);
  return true;  // for all else we go to true
}

}  // namespace cpplox::Evaluator
