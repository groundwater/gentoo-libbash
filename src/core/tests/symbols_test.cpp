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
/// \file symbols_test.cpp
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
  int_string.set_value("abc");
  EXPECT_EQ(0, int_string.get_value<int>());
}

TEST(symbol_test, array_variable)
{
  map<int, string> values = {{0, "1"}, {1, "2"}, {2, "3"}};

  // readonly array
  variable ro_array("foo", values, true);
  EXPECT_STREQ("foo", ro_array.get_name().c_str());
  EXPECT_STREQ("1", ro_array.get_value<string>(0).c_str());
  EXPECT_STREQ("2", ro_array.get_value<string>(1).c_str());
  EXPECT_STREQ("3", ro_array.get_value<string>(2).c_str());
  EXPECT_THROW(ro_array.set_value("4", 0), interpreter_exception);
  EXPECT_STREQ("1", ro_array.get_value<string>(0).c_str());

  // out of bound
  EXPECT_STREQ("", ro_array.get_value<string>(100).c_str());

  // normal array
  variable normal_array("foo", values);
  normal_array.set_value("5", 4);
  EXPECT_STREQ("1", normal_array.get_value<string>(0).c_str());
  EXPECT_STREQ("2", normal_array.get_value<string>(1).c_str());
  EXPECT_STREQ("3", normal_array.get_value<string>(2).c_str());
  EXPECT_STREQ("", normal_array.get_value<string>(3).c_str());
  EXPECT_STREQ("5", normal_array.get_value<string>(4).c_str());

  // get integer value
  EXPECT_EQ(3, normal_array.get_value<int>(2));
}

TEST(symbol_test, get_all_values)
{
  map<int, string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
  variable array("foo", values);
  vector<string> string_values;
  array.get_all_values(string_values);

  EXPECT_EQ(3, string_values.size());
  EXPECT_STREQ("1", string_values[0].c_str());
  EXPECT_STREQ("2", string_values[1].c_str());
  EXPECT_STREQ("3", string_values[2].c_str());

  variable a_string("foo", 10);
  // Won't clear the original vector
  a_string.get_all_values(string_values);
  EXPECT_EQ(4, string_values.size());
  EXPECT_STREQ("1", string_values[0].c_str());

  variable an_int("foo", 10);
  vector<int> int_values;
  an_int.get_all_values(int_values);
  EXPECT_EQ(1, int_values.size());
  EXPECT_EQ(10, int_values[0]);
}

TEST(symbol_test, is_null)
{
  variable var("foo", 10);
  EXPECT_FALSE(var.is_null());
  var.set_value("");
  EXPECT_TRUE(var.is_null());
  EXPECT_TRUE(variable("foo", "").is_null());
}

TEST(symbol_test, is_unset)
{
  map<int, string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
  variable array("foo", values);
  array.unset_value(1);
  EXPECT_TRUE(array.is_unset(1));
}

TEST(symbol_test, get_length)
{
  variable an_int("foo", 10);
  EXPECT_EQ(2, an_int.get_length());

  variable an_string("bar", "hello world");
  EXPECT_EQ(11, an_string.get_length());

  map<int, string> values = {{0, "1"}, {1, "2"}, {2, "hello"}};
  variable array("array", values);
  EXPECT_EQ(5, array.get_length(2));
  EXPECT_EQ(3, array.get_array_length());
}
