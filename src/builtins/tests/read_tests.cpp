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
/// \file read_tests.cpp
/// \brief series of unit tests for read builtin
///
#include <iostream>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>

#include "core/interpreter.h"
#include "cppbash_builtin.h"

using namespace std;

static void test_read(interpreter& walker, const string& input, std::initializer_list<string> args)
{
  stringstream test_input;
  test_input << input;
  cppbash_builtin::exec("read", args, std::cout, cerr, test_input, walker);
}

TEST(read_builtin_test, argument_assignment)
{
  interpreter walker;

  test_read(walker, "foo bar", {});
  EXPECT_STREQ("foo bar", walker.resolve<std::string>("REPLY").c_str());

  test_read(walker, "foo bar", {"var"});
  EXPECT_STREQ("foo bar", walker.resolve<std::string>("var").c_str());

  test_read(walker, "foo bar", {"var1", "var2"});
  EXPECT_STREQ("foo", walker.resolve<std::string>("var1").c_str());
  EXPECT_STREQ("bar", walker.resolve<std::string>("var2").c_str());

  test_read(walker, "1 2 3 4", {"var1", "var2"});
  EXPECT_STREQ("1", walker.resolve<std::string>("var1").c_str());
  EXPECT_STREQ("2 3 4", walker.resolve<std::string>("var2").c_str());

  test_read(walker, "foo", {"var1", "var2"});
  EXPECT_STREQ("foo", walker.resolve<std::string>("var1").c_str());
  EXPECT_STREQ("", walker.resolve<std::string>("var2").c_str());

  test_read(walker, "foo    bar", {"var"});
  EXPECT_STREQ("foo    bar", walker.resolve<std::string>("var").c_str());
}

TEST(read_builtin_test, line_continuation)
{
  interpreter walker;

  test_read(walker, "foo\\\nbar", {});
  EXPECT_STREQ("foobar", walker.resolve<std::string>("REPLY").c_str());

  test_read(walker, "foo \\\n bar", {});
  EXPECT_STREQ("foo  bar", walker.resolve<std::string>("REPLY").c_str());
}
