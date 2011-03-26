/*
   Please use git log for copyright holder and year information

   This file is part of libbash.

   libbash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   libbash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with libbash.  If not, see <http://www.gnu.org/licenses/>.
*/
///
/// \file interpreter.h
/// \author Mu Qiao
/// \brief implementation for bash interpreter (visitor pattern)
///

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <cmath>

#include <memory>
#include <string>

#include <antlr3basetree.h>

#include "bashastLexer.h"
#include "core/symbols.hpp"

///
/// \class interpreter
/// \brief implementation for bash interpreter
///
class interpreter{
  /// \var private::members
  /// \brief global symbol table
  scope members;
public:

  /// \brief parse the text value of a tree to integer
  /// \param the target tree
  /// \return the parsed value
  static int parse_int(ANTLR3_BASE_TREE* tree)
  {
    return tree->getText(tree)->toInt32(tree->getText(tree));
  }

  /// \brief a helper function that get the string value
  ///        of the given pANTLR3_BASE_TREE node.
  /// \param the target tree node
  /// \return the value of node->text
  static std::string get_string(pANTLR3_BASE_TREE node)
  {
    pANTLR3_COMMON_TOKEN token = node->getToken(node);
    // Use reinterpret_cast here because we have to cast C code.
    // The real type here is int64_t which is used as a pointer.
    return std::string(reinterpret_cast<const char *>(token->start),
                       token->stop - token->start + 1);
  }

  /// \brief perform logic or
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int logicor(int left, int right)
  {
    return left || right;
  }

  /// \brief perform logic and
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int logicand(int left, int right)
  {
    return left && right;
  }

  /// \brief perform bitwise or
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int bitwiseor(int left, int right)
  {
    return left | right;
  }

  /// \brief perform bitwise and
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int bitwiseand(int left, int right)
  {
    return left & right;
  }

  /// \brief perform bitwise xor
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int bitwisexor(int left, int right)
  {
    return left ^ right;
  }

  /// \brief perform left or equal to
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int less_equal_than(int left, int right)
  {
    return left <= right;
  }

  /// \brief perform greater or equal to
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int greater_equal_than(int left, int right)
  {
    return left >= right;
  }

  /// \brief perform less than
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int less_than(int left, int right)
  {
    return left < right;
  }

  /// \brief perform greater than
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int greater_than(int left, int right)
  {
    return left > right;
  }

  /// \brief perform left shift
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int left_shift(int left, int right)
  {
    return left << right;
  }

  /// \brief perform right shift
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int right_shift(int left, int right)
  {
    return left >> right;
  }

  /// \brief perform plus
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int plus(int left, int right)
  {
    return left + right;
  }

  /// \brief perform minus
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int minus(int left, int right)
  {
    return left - right;
  }

  /// \brief perform multiply
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int multiply(int left, int right)
  {
    return left * right;
  }

  /// \brief perform divide
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int divide(int left, int right)
  {
    // We are not handling division by zero right now
    return left / right;
  }

  /// \brief perform modulo
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int mod(int left, int right)
  {
    return left % right;
  }

  /// \brief perform exponential operation
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int exp(int left, int right)
  {
    return pow(left, right);
  }

  /// \brief perform logic negation
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int negation(int element)
  {
    return !element;
  }

  /// \brief perform bitwise negation
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int bitwise_negation(int element)
  {
    return ~element;
  }


  /// \brief perform the ternary operator
  /// \param the condition
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int arithmetic_condition(int cnd, int left, int right)
  {
    return (cnd? left : right);
  }

  /// \brief perform pre-increment
  /// \param the variable name
  /// \return the increased value
  int pre_incr(const std::string& name)
  {
    int value = resolve<int>(name);
    set_value(name, ++value);
    return value;
  }

  /// \brief perform pre-decrement
  /// \param the variable name
  /// \return the decreased value
  int pre_decr(const std::string& name)
  {
    int value = resolve<int>(name);
    set_value(name, --value);
    return value;
  }

  /// \brief perform post-increment
  /// \param the variable name
  /// \return the original value
  int post_incr(const std::string& name)
  {
    int value = resolve<int>(name);
    set_value(name, value + 1);
    return value;
  }

  /// \brief perform post-decrement
  /// \param the variable name
  /// \return the original value
  int post_decr(const std::string& name)
  {
    int value = resolve<int>(name);
    set_value(name, value - 1);
    return value;
  }

  /// \brief resolve any variable
  /// \param variable name
  /// \return the value of the variable, call default constructor if
  //          it's undefined
  template <typename T>
  T resolve(const std::string& name)
  {
    std::shared_ptr<symbol> value = members.resolve(name);
    if(!value)
      return T();
    return std::static_pointer_cast<variable>(value)->get_value<T>();
  }

  /// \brief update the variable value, raise interpreter_exception if
  ///        it's readonly, do thing if the variable doesn't exist
  /// \param variable name
  /// \param new value
  template <typename T>
  void set_value(const std::string& name, const T& new_value)
  {
    std::shared_ptr<symbol> value = members.resolve(name);
    if(!value)
      return;
    std::static_pointer_cast<variable>(value)->set_value(new_value);
  }

  /// \brief define a new variable
  /// \param the name of the variable
  /// \param the value of the variable
  /// \param whether it's readonly, default is false
  template <typename T>
  void define(const std::string& name,
              const T& value,
              bool readonly=false)
  {
    std::shared_ptr<variable> target(
        new variable(name, value, readonly));
    members.define(target);
  }
};
#endif
