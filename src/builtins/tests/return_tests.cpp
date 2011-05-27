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
/// \file return_tests.cpp
/// \brief series of unit tests for return builtin
///
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

#include "builtins/builtin_exceptions.h"
#include "core/interpreter.h"
#include "cppbash_builtin.h"

TEST(return_builtin_test, bad_argument)
{
  interpreter walker;
  EXPECT_THROW(cppbash_builtin::exec("return", {"abc"}, std::cout, std::cerr, std::cin, walker), boost::bad_lexical_cast);
  EXPECT_THROW(cppbash_builtin::exec("return", {"abc", "def"}, std::cout, std::cerr, std::cin, walker), interpreter_exception);
}

TEST(return_builtin_test, bad_location)
{
  interpreter walker;
  EXPECT_THROW(cppbash_builtin::exec("return", {}, std::cout, std::cerr, std::cin, walker), return_exception);
}
