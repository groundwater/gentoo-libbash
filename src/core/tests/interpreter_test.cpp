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
/// \file interpreter_test.cpp
/// \author Mu Qiao
/// \brief series of unit tests for interpreter.
///

#include <gtest/gtest.h>

#include "core/interpreter.h"

using namespace std;

TEST(interpreter, define_resolve_int)
{
  interpreter walker;
  walker.define("aint", 4);
  EXPECT_EQ(4, walker.resolve<int>("aint"));
  EXPECT_EQ(0, walker.resolve<int>("undefined"));
}

TEST(interpreter, define_resolve_string)
{
  interpreter walker;
  walker.define("astring", "hello");
  EXPECT_STREQ("hello", walker.resolve<string>("astring").c_str());
  EXPECT_STREQ("", walker.resolve<string>("undefined").c_str());
}

TEST(interpreter, define_resolve_array)
{
  interpreter walker;
  std::map<int, std::string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
  walker.define("array", values);
  EXPECT_STREQ("1", walker.resolve<string>("array", 0).c_str());
  EXPECT_STREQ("2", walker.resolve<string>("array", 1).c_str());
  EXPECT_STREQ("3", walker.resolve<string>("array", 2).c_str());
  EXPECT_STREQ("", walker.resolve<string>("undefined",100).c_str());

  walker.define("partial", 10, false, false, 8);
  EXPECT_EQ(1, walker.get_array_length("partial"));
  EXPECT_EQ(10, walker.resolve<int>("partial", 8));
}

TEST(interpreter, is_unset_or_null)
{
  interpreter walker;
  walker.define("foo", "hello");
  EXPECT_FALSE(walker.is_unset_or_null("foo", 0));
  walker.define("foo", "hello", false, true);
  EXPECT_TRUE(walker.is_unset_or_null("foo", 0));

  std::map<int, std::string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
  walker.define("bar", values);
  EXPECT_FALSE(walker.is_unset_or_null("bar", 0));
  EXPECT_FALSE(walker.is_unset_or_null("bar", 1));
  EXPECT_FALSE(walker.is_unset_or_null("bar", 2));
  EXPECT_TRUE(walker.is_unset_or_null("bar", 3));
}

TEST(interpreter, is_unset)
{
  interpreter walker;
  walker.define("foo", "hello");
  EXPECT_FALSE(walker.is_unset("foo"));
  EXPECT_TRUE(walker.is_unset("bar"));
}

TEST(interpreter, set_int_value)
{
  interpreter walker;
  walker.define("aint", 4);
  EXPECT_EQ(10, walker.set_value("aint", 10));
  EXPECT_EQ(10, walker.resolve<int>("aint"));
  EXPECT_EQ(10, walker.set_value("undefined", 10));
  EXPECT_EQ(10, walker.resolve<int>("undefined"));

  walker.define("aint_ro", 4, true);
  EXPECT_THROW(walker.set_value("aint_ro", 10),
               interpreter_exception);
  EXPECT_EQ(4, walker.resolve<int>("aint_ro"));
}

TEST(interpreter, set_string_value)
{
  interpreter walker;
  walker.define("astring", "hi");
  EXPECT_STREQ("hello", walker.set_value<string>("astring", "hello").c_str());
  EXPECT_STREQ("hello", walker.resolve<string>("astring").c_str());
  EXPECT_STREQ("hello", walker.set_value<string>("undefined", "hello").c_str());
  EXPECT_STREQ("hello", walker.resolve<string>("undefined").c_str());

  walker.define("astring_ro", "hi", true);
  EXPECT_THROW(walker.set_value<string>("astring_ro", "hello"),
               interpreter_exception);
  EXPECT_STREQ("hi", walker.resolve<string>("astring_ro").c_str());
}

TEST(interpreter, set_array_value)
{
  interpreter walker;
  std::map<int, std::string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
  walker.define("array", values);
  EXPECT_STREQ("2", walker.set_value<string>("array", "2", 0).c_str());
  EXPECT_STREQ("2", walker.resolve<string>("array", 0).c_str());
  EXPECT_STREQ("out_of_bound", walker.set_value<string>("array", "out_of_bound", 10).c_str());
  EXPECT_STREQ("out_of_bound", walker.resolve<string>("array",10).c_str());

  walker.define("ro_array", values, true);
  EXPECT_THROW(walker.set_value<string>("ro_array", "hello", 1),
               interpreter_exception);
  EXPECT_STREQ("2", walker.resolve<string>("ro_array", 1).c_str());
}

TEST(interpreter, get_array_values)
{
  interpreter walker;
  std::map<int, std::string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
  walker.define("array", values);

  std::vector<int> array_values;
  walker.resolve_array("array", array_values);
  EXPECT_EQ(1, array_values[0]);
  EXPECT_EQ(2, array_values[1]);
  EXPECT_EQ(3, array_values[2]);
}

TEST(interperter, substring_expansion_exception)
{
  interpreter walker;
  EXPECT_THROW(walker.do_substring_expansion("", 0, -1, 0), interpreter_exception);
}
