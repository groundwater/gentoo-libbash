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
/// \file echo_tests.cpp
/// \brief series of unit tests for echo built in
///
#include <iostream>

#include <gtest/gtest.h>

#include "core/interpreter.h"
#include "cppbash_builtin.h"

using namespace std;

TEST(boolean_builtin_test, true)
{
  interpreter walker;
  int result = cppbash_builtin::exec("true", {}, std::cout, std::cerr, std::cin, walker);
  ASSERT_EQ(0, result);
}

TEST(boolean_builtin_test, false)
{
  interpreter walker;
  int result = cppbash_builtin::exec("false", {}, std::cout, std::cerr, std::cin, walker);
  ASSERT_EQ(1, result);
}
