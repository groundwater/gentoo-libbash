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
/// \file declare_tests.cpp
/// \brief series of unit tests for declare built in
/// \author Mu Qiao Eloe
///
#include <iostream>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>

#include "core/bash_ast.h"
#include "core/interpreter.h"
#include "cppbash_builtin.h"

using namespace std;

static void test_declare(const string& expected, std::initializer_list<string> args)
{
  stringstream test_output;
  interpreter walker;
  cppbash_builtin::exec("declare",args,cout,test_output,cin,walker);
  EXPECT_EQ(expected, test_output.str());
}

TEST(declare_builtin_test, invalid_arguments)
{
  test_declare("Arguments required for declare\n", {});
  test_declare("Multiple arguments are not supported\n", {"-ap"});
  test_declare("Invalid option for declare builtin\n", {"_a"});
  test_declare("Unrecognized option for declare: -L\n", {"-L"});
}

TEST(declare_builtin_test, _F)
{
  stringstream expression("function foo() { :; }; function bar() { :; }");
  interpreter walker;
  bash_ast ast(expression);
  ast.interpret_with(walker);

  stringstream test_output1;
  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"-F", "foo"}, test_output1, cerr, cin, walker));
  EXPECT_EQ("foo\n", test_output1.str());

  stringstream test_output2;
  EXPECT_EQ(1, cppbash_builtin::exec("declare", {"-F", "foo", "bar", "test"}, test_output2, cerr, cin, walker));
  EXPECT_EQ("foo\nbar\n", test_output2.str());

  stringstream test_output3;
  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"+F", "foo", "bar", "test"}, test_output3, cerr, cin, walker));
  EXPECT_EQ("", test_output3.str());

  stringstream test_output4;
  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"-F"}, test_output3, cerr, cin, walker));
  EXPECT_EQ("declare -f bar\ndeclare -f foo\n", test_output3.str());
}

#define TEST_DECLARE(name, expected, ...) \
	TEST(declare_builtin_test, name) { test_declare(expected, {__VA_ARGS__}); }

TEST_DECLARE(_a, "declare -a is not supported yet\n", "-a", "world")
TEST_DECLARE(_A, "declare -A is not supported yet\n", "-A", "world")
TEST_DECLARE(_f, "declare -f is not supported yet\n", "-f", "world")
TEST_DECLARE(_i, "declare -i is not supported yet\n", "-i", "world")
TEST_DECLARE(_l, "declare -l is not supported yet\n", "-l", "world")
TEST_DECLARE(_r, "declare -r is not supported yet\n", "-r", "world")
TEST_DECLARE(_t, "declare -t is not supported yet\n", "-t", "world")
TEST_DECLARE(_u, "declare -u is not supported yet\n", "-u", "world")
TEST_DECLARE(_x, "declare -x is not supported yet\n", "-x", "world")
TEST_DECLARE(_p, "declare -p is not supported yet\n", "-p", "world")
TEST_DECLARE(pa, "declare +a is not supported yet\n", "+a", "world")
TEST_DECLARE(pA, "declare +A is not supported yet\n", "+A", "world")
TEST_DECLARE(pf, "declare +f is not supported yet\n", "+f", "world")
TEST_DECLARE(pi, "declare +i is not supported yet\n", "+i", "world")
TEST_DECLARE(pl, "declare +l is not supported yet\n", "+l", "world")
TEST_DECLARE(pr, "declare +r is not supported yet\n", "+r", "world")
TEST_DECLARE(pt, "declare +t is not supported yet\n", "+t", "world")
TEST_DECLARE(pu, "declare +u is not supported yet\n", "+u", "world")
TEST_DECLARE(px, "declare +x is not supported yet\n", "+x", "world")
