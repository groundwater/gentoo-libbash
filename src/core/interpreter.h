/*
   Copyright 2011 Mu Qiao

   This file is part of libbash.

   libbash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   libbash is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of
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
public:
  /// \var public::members
  /// \brief global symbol table
  scope members;

  /// \brief parse the text value of a tree to integer
  /// \param the target tree
  /// \return the parsed value
  static int parse_int(ANTLR3_BASE_TREE* tree)
  {
    return tree->getText(tree)->toInt32(tree->getText(tree));
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
};
#endif
