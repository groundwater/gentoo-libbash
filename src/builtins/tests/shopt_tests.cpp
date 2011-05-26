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

TEST(shopt_builtin_test, disable_extglob)
{
  interpreter walker;
  EXPECT_EQ(1, cppbash_builtin::exec("shopt", {"-u", "not exist"}, std::cout, std::cerr, std::cin, walker));

  walker.set_option("autocd", true);
  EXPECT_EQ(0, cppbash_builtin::exec("shopt", {"-u", "autocd", "cdspell"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_FALSE(walker.get_option("autocd"));
  EXPECT_FALSE(walker.get_option("cdspell"));
}

TEST(shopt_builtin_test, enable_extglob)
{
  interpreter walker;
  EXPECT_EQ(1, cppbash_builtin::exec("shopt", {"-s", "not exist"}, std::cout, std::cerr, std::cin, walker));

  EXPECT_EQ(0, cppbash_builtin::exec("shopt", {"-s", "autocd", "cdspell"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_TRUE(walker.get_option("autocd"));
  EXPECT_TRUE(walker.get_option("cdspell"));
}

static void test_shopt_builtin(const std::string& expected, const std::vector<std::string>& args, int status)
{
  std::stringstream output;
  interpreter walker;
  EXPECT_EQ(status, cppbash_builtin::exec("shopt", args, std::cout, output, std::cin, walker));
  EXPECT_STREQ(expected.c_str(), output.str().c_str());
}

TEST(shopt_builtin_test, invalid_argument)
{
  test_shopt_builtin("Arguments required for shopt\n", {}, 1);
  test_shopt_builtin("Multiple arguments are not supported\n", {"-so"}, 1);
  test_shopt_builtin("shopt -q is not supported yet\n", {"-q"}, 1);
  test_shopt_builtin("Unrecognized option for shopt: -d\n", {"-d"}, 1);
}
