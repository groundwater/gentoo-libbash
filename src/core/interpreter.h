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
#include <functional>

#include <antlr3basetree.h>

#include "libbashLexer.h"
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

  ///
  /// \brief return the number of variables
  /// \return the number of variables
  scope::size_type size()
  {
    return members.size();
  }

  ///
  /// \brief return an iterator referring to the first variable
  /// \return iterator referring to the first variable
  scope::iterator begin()
  {
    return members.begin();
  }

  ///
  /// \brief return a const iterator referring to the first variable
  /// \return const iterator referring to the first variable
  scope::const_iterator begin() const
  {
    return members.begin();
  }

  ///
  /// \brief return an iterator referring to the next element after the
  ///        last variable
  /// \return iterator referring to he next element after the last variable
  scope::iterator end()
  {
    return members.end();
  }

  ///
  /// \brief return a const iterator referring to the next element after
  ///        the last variable
  /// \return const iterator referring to he next element after the last
  ///         variable
  scope::const_iterator end() const
  {
    return members.end();
  }

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
    // The tree walker may send null pointer here, so return an empty
    // string if that's the case.
    if(!token->start)
      return "";
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

  /// \brief assign with an operator (for example multiply)
  /// \param a function object to do an operation while assigning
  /// \param the name of the variable
  /// \param the value to assign
  /// \return the new value of the variable
  int assign(std::function<int(int,int)> f, const std::string& name, int value)
  {
    int new_value = f(resolve<int>(name), value);
    set_value(name, new_value);
    return new_value;
  }

  /// \brief resolve any variable
  /// \param variable name
  /// \return the value of the variable, call default constructor if
  //          it's undefined
  template <typename T>
  T resolve(const std::string& name)
  {
    std::shared_ptr<variable> value = members.resolve(name);
    if(!value)
      return T();
    return value->get_value<T>();
  }

  /// \brief check whether the value of the variable is null, return true
  ///        if the variable is undefined
  /// \param variable name
  /// \return whether the value of the variable is null
  bool is_null(const std::string& name)
  {
    std::shared_ptr<variable> value = members.resolve(name);
    if(value)
      return value->is_null();
    else
      return true;
  }

  /// \brief check whether the value of the variable is unset
  /// \param variable name
  /// \return whether the value of the variable is unset
  bool is_unset(const std::string& name)
  {
    return !members.resolve(name);
  }

  /// \brief update the variable value, raise interpreter_exception if
  ///        it's readonly, will define the variable if it doesn't exist
  /// \param variable name
  /// \param new value
  /// \return the new value of the variable
  template <typename T>
  const T& set_value(const std::string& name, const T& new_value)
  {
    std::shared_ptr<variable> value = members.resolve(name);
    if(!value)
      define(name, new_value, false);
    else
      value->set_value(new_value);
    return new_value;
  }

  /// \brief define a new variable
  /// \param the name of the variable
  /// \param the value of the variable
  /// \param whether it's readonly, default is false
  /// \param whether it's null, default is false
  template <typename T>
  void define(const std::string& name,
              const T& value,
              bool readonly=false,
              bool is_null=false)
  {
    std::shared_ptr<variable> target(
        new variable(name, value, readonly, is_null));
    members.define(target);
  }

  /// \brief perform ${parameter:−word} expansion
  /// \param the name of the parameter
  /// \param the value of the word
  /// \return the expansion result
  const std::string do_default_expansion(const std::string& name,
                                         const std::string& value)
  {
    return (is_null(name)? value : resolve<std::string>(name));
  }
};
#endif
