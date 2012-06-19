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
///
#include <iostream>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>

#include "core/bash_ast.h"
#include "core/interpreter.h"
#include "cppbash_builtin.h"
#include "exceptions.h"

using namespace std;

namespace
{
  template <typename T>
  void test_declare(const string& expected, std::initializer_list<string> args)
  {
    stringstream test_output;
    interpreter walker;
    try
    {
      cppbash_builtin::exec("declare",args,cout,test_output,cin,walker);
      FAIL();
    }
    catch(T& e)
    {
      EXPECT_EQ(expected, e.what());
    }
  }
}

TEST(declare_builtin_test, invalid_arguments)
{
  test_declare<libbash::illegal_argument_exception>("declare: arguments required", {});
  test_declare<libbash::unsupported_exception>("declare: -ap is not supported yet", {"-ap"});
  test_declare<libbash::illegal_argument_exception>("declare: unrecognized option: -L", {"-L"});
}

TEST(declare_built_test, declarations)
{
  interpreter walker;

  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"var"}, cout, cerr, cin, walker));

  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"foo1=bar"}, cout, cerr, cin, walker));
  EXPECT_STREQ("bar", walker.resolve<std::string>("foo1").c_str());

  walker.define("foo2", "bar");
  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"foo2"}, cout, cerr, cin, walker));
  EXPECT_STREQ("bar", walker.resolve<std::string>("foo2").c_str());

  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"var1=foo var2 var3=bar"}, cout, cerr, cin, walker));
  EXPECT_STREQ("foo", walker.resolve<std::string>("var1").c_str());
  EXPECT_STREQ("", walker.resolve<std::string>("var2").c_str());
  EXPECT_STREQ("bar", walker.resolve<std::string>("var3").c_str());

  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"var=\"foo bar\""}, cout, cerr, cin, walker));
  EXPECT_STREQ("foo bar", walker.resolve<std::string>("var").c_str());
}

TEST(declare_builtin_test, _F)
{
  stringstream expression("function foo() { :; }; function bar() { :; }");
  interpreter walker;
  bash_ast ast(expression);
  ast.interpret_with(walker);

  stringstream test_output1;
  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"-F foo"}, test_output1, cerr, cin, walker));
  EXPECT_EQ("foo\n", test_output1.str());

  stringstream test_output2;
  EXPECT_EQ(1, cppbash_builtin::exec("declare", {"-F foo bar test"}, test_output2, cerr, cin, walker));
  EXPECT_EQ("foo\nbar\n", test_output2.str());

  stringstream test_output3;
  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"+F foo bar test"}, test_output3, cerr, cin, walker));
  EXPECT_EQ("", test_output3.str());

  stringstream test_output4;
  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"-F"}, test_output3, cerr, cin, walker));
  EXPECT_EQ("declare -f bar\ndeclare -f foo\n", test_output3.str());
}

TEST(declare_built_test, _p)
{
  interpreter walker;
  walker.define("foo", "bar");

  stringstream test_output1;
  EXPECT_EQ(0, cppbash_builtin::exec("declare", {"-p foo"}, test_output1, cerr, cin, walker));
  EXPECT_EQ("declare -- foo=\"bar\"\n", test_output1.str());

  stringstream test_output2;
  EXPECT_EQ(1, cppbash_builtin::exec("declare", {"-p bar test"}, test_output2, cerr, cin, walker));
  EXPECT_EQ("-bash: declare: bar: not found\n-bash: declare: test: not found\n", test_output2.str());
}

#define TEST_DECLARE(name, expected, ...) \
	TEST(declare_builtin_test, name) { test_declare<libbash::unsupported_exception>(expected, {__VA_ARGS__}); }

TEST_DECLARE(_a, "declare -a is not supported yet", "-a", "world")
TEST_DECLARE(_A, "declare -A is not supported yet", "-A", "world")
TEST_DECLARE(_f, "declare -f is not supported yet", "-f", "world")
TEST_DECLARE(_i, "declare -i is not supported yet", "-i", "world")
TEST_DECLARE(_l, "declare -l is not supported yet", "-l", "world")
TEST_DECLARE(_r, "declare -r is not supported yet", "-r", "world")
TEST_DECLARE(_t, "declare -t is not supported yet", "-t", "world")
TEST_DECLARE(_u, "declare -u is not supported yet", "-u", "world")
TEST_DECLARE(_x, "declare -x is not supported yet", "-x", "world")
TEST_DECLARE(pa, "declare +a is not supported yet", "+a", "world")
TEST_DECLARE(pA, "declare +A is not supported yet", "+A", "world")
TEST_DECLARE(pf, "declare +f is not supported yet", "+f", "world")
TEST_DECLARE(pi, "declare +i is not supported yet", "+i", "world")
TEST_DECLARE(pl, "declare +l is not supported yet", "+l", "world")
TEST_DECLARE(pr, "declare +r is not supported yet", "+r", "world")
TEST_DECLARE(pt, "declare +t is not supported yet", "+t", "world")
TEST_DECLARE(pu, "declare +u is not supported yet", "+u", "world")
TEST_DECLARE(px, "declare +x is not supported yet", "+x", "world")
