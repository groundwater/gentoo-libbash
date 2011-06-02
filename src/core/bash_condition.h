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
/// \file bash_condition.h
/// \brief implementation for Bash Conditional Expressions
///
#ifndef LIBBASH_CORE_BASH_CONDITION_H_
#define LIBBASH_CORE_BASH_CONDITION_H_

#include <string>

namespace internal
{
  bool test_unary(char op, const std::string& target);

  bool test_binary(const std::string& op,
                   const std::string& lhs,
                   const std::string& rhs);
}

#endif
