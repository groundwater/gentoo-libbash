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
/// \file continue_tests.cpp
/// \brief series of unit tests for continue builtin
///
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

#include "builtins/builtin_exceptions.h"
#include "core/interpreter.h"
#include "cppbash_builtin.h"

TEST(continue_builtin_test, bad_argument)
{
  interpreter walker;
  EXPECT_THROW(cppbash_builtin::exec("continue", {"abc"}, std::cout, std::cerr, std::cin, walker), libbash::illegal_argument_exception);
  EXPECT_THROW(cppbash_builtin::exec("continue", {"1", "2"}, std::cout, std::cerr, std::cin, walker), libbash::illegal_argument_exception);
  EXPECT_THROW(cppbash_builtin::exec("continue", {"0"}, std::cout, std::cerr, std::cin, walker), libbash::illegal_argument_exception);
  EXPECT_THROW(cppbash_builtin::exec("continue", {"-1"}, std::cout, std::cerr, std::cin, walker), libbash::illegal_argument_exception);
}

TEST(continue_builtin_test, throw_exception)
{
  interpreter walker;
  try
  {
    cppbash_builtin::exec("continue", {}, std::cout, std::cerr, std::cin, walker);
    FAIL();
  }
  catch(continue_exception& e)
  {
    EXPECT_NO_THROW(e.rethrow_unless_correct_frame());
  }

  try
  {
    cppbash_builtin::exec("continue", {"2"}, std::cout, std::cerr, std::cin, walker);
    FAIL();
  }
  catch(continue_exception& e)
  {
    try
    {
      e.rethrow_unless_correct_frame();
      FAIL();
    }
    catch(continue_exception& e)
    {
      EXPECT_NO_THROW(e.rethrow_unless_correct_frame());
    }
  }
}
