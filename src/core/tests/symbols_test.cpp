/*
Copyright 2011 Mu Qiao

This file is part of libbash.

libbash is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

libbash is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libbash.  If not, see <http://www.gnu.org/licenses/>.
*/
///
/// \file symbols_test.cpp
/// \author Mu Qiao
/// \brief series of unit tests for symbols and symbol table.
///

#include <gtest/gtest.h>

#include "core/symbols.hpp"

using namespace std;

TEST(symbol_test, int_variable)
{
  // readonly integer
  variable ro_integer("integer", 10, true);
  EXPECT_STREQ("integer", ro_integer.get_name().c_str());
  EXPECT_EQ(10, ro_integer.get_value<int>());
  EXPECT_THROW(ro_integer.set_value(100), interpreter_exception);
  EXPECT_EQ(10, ro_integer.get_value<int>());

  // normal only integer
  variable normal_integer("integer", 10);
  normal_integer.set_value(100);
  EXPECT_EQ(100, normal_integer.get_value<int>());

  // get string value of an integer
  EXPECT_STREQ("100", normal_integer.get_value<string>().c_str());
}

TEST(symbol_test, string_variable)
{
  // readonly string
  variable ro_string("string", "hello", true);
  EXPECT_STREQ("string", ro_string.get_name().c_str());
  EXPECT_STREQ("hello", ro_string.get_value<string>().c_str());
  EXPECT_THROW(ro_string.set_value("hello world"), interpreter_exception);
  EXPECT_STREQ("hello", ro_string.get_value<string>().c_str());

  // normal string
  variable normal_string("string", "hello");
  normal_string.set_value("hello world");
  EXPECT_STREQ("hello world", normal_string.get_value<string>().c_str());

  // string contains integer value
  variable int_string("string", "123");
  EXPECT_EQ(123, int_string.get_value<int>());
}

TEST(scope_test, define_resolve)
{
  scope members;
  auto an_int = shared_ptr<variable>(new variable("integer_symbol", 100));
  members.define(an_int);
  EXPECT_EQ(an_int, members.resolve("integer_symbol"));
}