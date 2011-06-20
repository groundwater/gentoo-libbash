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
/// \file break_tests.cpp
/// \brief series of unit tests for break builtin
///
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

#include "builtins/builtin_exceptions.h"
#include "core/interpreter.h"
#include "cppbash_builtin.h"

TEST(break_builtin_test, bad_argument)
{
  interpreter walker;
  EXPECT_THROW(cppbash_builtin::exec("break", {"abc"}, std::cout, std::cerr, std::cin, walker), libbash::interpreter_exception);
  EXPECT_THROW(cppbash_builtin::exec("break", {"1", "2"}, std::cout, std::cerr, std::cin, walker), libbash::interpreter_exception);
  EXPECT_THROW(cppbash_builtin::exec("break", {"0"}, std::cout, std::cerr, std::cin, walker), libbash::interpreter_exception);
  EXPECT_THROW(cppbash_builtin::exec("break", {"-1"}, std::cout, std::cerr, std::cin, walker), libbash::interpreter_exception);
}

TEST(break_builtin_test, throw_exception)
{
  interpreter walker;
  try
  {
    cppbash_builtin::exec("break", {}, std::cout, std::cerr, std::cin, walker);
    FAIL();
  }
  catch(break_exception& e)
  {
    EXPECT_NO_THROW(e.rethrow_unless_correct_frame());
  }

  try
  {
    cppbash_builtin::exec("break", {"2"}, std::cout, std::cerr, std::cin, walker);
    FAIL();
  }
  catch(break_exception& e)
  {
    try
    {
      e.rethrow_unless_correct_frame();
      FAIL();
    }
    catch(break_exception& e)
    {
      EXPECT_NO_THROW(e.rethrow_unless_correct_frame());
    }
  }
}
