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
/// \file shopt_tests.cpp
/// \brief series of unit tests for shopt builtin
///
#include <gtest/gtest.h>

#include "builtins/builtin_exceptions.h"
#include "core/interpreter.h"
#include "cppbash_builtin.h"

static void test_shopt_builtin(const std::string& expected, const std::vector<std::string>& args)
{
  std::stringstream output;
  interpreter walker;
  try
  {
    cppbash_builtin::exec("shopt", args, std::cout, output, std::cin, walker);
    FAIL();
  }
  catch(interpreter_exception& e)
  {
    EXPECT_STREQ(expected.c_str(), e.what());
  }
}


TEST(shopt_builtin_test, disable_extglob)
{
  test_shopt_builtin("not exist is not a valid bash option", {"-u", "not exist"});

  interpreter walker;
  walker.set_option("autocd", true);
  EXPECT_EQ(0, cppbash_builtin::exec("shopt", {"-u", "autocd", "cdspell"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_FALSE(walker.get_option("autocd"));
  EXPECT_FALSE(walker.get_option("cdspell"));
}

TEST(shopt_builtin_test, enable_extglob)
{
  test_shopt_builtin("not exist is not a valid bash option", {"-s", "not exist"});

  interpreter walker;
  EXPECT_EQ(0, cppbash_builtin::exec("shopt", {"-s", "autocd", "cdspell"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_TRUE(walker.get_option("autocd"));
  EXPECT_TRUE(walker.get_option("cdspell"));
}

TEST(shopt_builtin_test, invalid_argument)
{
  test_shopt_builtin("Arguments required for shopt", {});
  test_shopt_builtin("Multiple arguments are not supported", {"-so"});
  test_shopt_builtin("shopt -q is not supported yet", {"-q"});
  test_shopt_builtin("Unrecognized option for shopt: -d", {"-d"});
}
