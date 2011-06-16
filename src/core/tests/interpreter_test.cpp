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
/// \brief series of unit tests for interpreter.
///

#include <gtest/gtest.h>

#include "core/interpreter.h"
#include "core/unset_exception.h"

using namespace std;

TEST(interpreter, define_resolve_int)
{
  interpreter walker;
  walker.define("aint", 4);
  EXPECT_EQ(4, walker.resolve<int>("aint"));
  EXPECT_EQ(0, walker.resolve<int>("undefined"));
  EXPECT_EQ(0, walker.resolve<int>(""));
}

TEST(interpreter, define_resolve_string)
{
  interpreter walker;
  walker.define("astring", "hello");
  EXPECT_STREQ("hello", walker.resolve<string>("astring").c_str());
  EXPECT_STREQ("", walker.resolve<string>("undefined").c_str());
  EXPECT_STREQ("", walker.resolve<string>("").c_str());
}

TEST(interpreter, define_resolve_array)
{
  interpreter walker;
  std::map<unsigned, std::string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
  walker.define("array", values);
  EXPECT_STREQ("1", walker.resolve<string>("array", 0).c_str());
  EXPECT_STREQ("2", walker.resolve<string>("array", 1).c_str());
  EXPECT_STREQ("3", walker.resolve<string>("array", 2).c_str());
  EXPECT_STREQ("", walker.resolve<string>("undefined",100).c_str());

  walker.define("partial", 10, false, 8);
  EXPECT_EQ(1, walker.get_array_length("partial"));
  EXPECT_EQ(10, walker.resolve<int>("partial", 8));

  EXPECT_EQ(0, walker.get_array_length("not exist"));
}

TEST(interpreter, is_unset_or_null)
{
  interpreter walker;
  walker.define("foo", "hello");
  EXPECT_FALSE(walker.is_unset_or_null("foo", 0));
  walker.define("foo", "hello", false, true);
  EXPECT_TRUE(walker.is_unset_or_null("foo", 0));

  std::map<unsigned, std::string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
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
               libbash::interpreter_exception);
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
               libbash::interpreter_exception);
  EXPECT_STREQ("hi", walker.resolve<string>("astring_ro").c_str());
}

TEST(interpreter, set_array_value)
{
  interpreter walker;
  std::map<unsigned, std::string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
  walker.define("array", values);
  EXPECT_STREQ("2", walker.set_value<string>("array", "2", 0).c_str());
  EXPECT_STREQ("2", walker.resolve<string>("array", 0).c_str());
  EXPECT_STREQ("out_of_bound", walker.set_value<string>("array", "out_of_bound", 10).c_str());
  EXPECT_STREQ("out_of_bound", walker.resolve<string>("array",10).c_str());

  walker.define("ro_array", values, true);
  EXPECT_THROW(walker.set_value<string>("ro_array", "hello", 1),
               libbash::interpreter_exception);
  EXPECT_STREQ("2", walker.resolve<string>("ro_array", 1).c_str());
}

TEST(interpreter, get_array_values)
{
  interpreter walker;
  std::map<unsigned, std::string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
  walker.define("array", values);

  std::vector<int> array_values;
  EXPECT_TRUE(walker.resolve_array("array", array_values));
  EXPECT_EQ(1, array_values[0]);
  EXPECT_EQ(2, array_values[1]);
  EXPECT_EQ(3, array_values[2]);
  EXPECT_FALSE(walker.resolve_array("undefined", array_values));
}

TEST(interpreter, unset_arrays)
{
  interpreter walker;
  std::map<unsigned, std::string> values = {{0, "1"}, {1, "2"}, {2, "3"}};
  walker.define("array", values);
  walker.define("ro_array", values, true);
  interpreter::local_scope temp_scope(walker);
  values[0] = "local";
  walker.define_local("array", values);
  walker.define_local("ro_local_array", values, true);

  // unset arrays
  EXPECT_STREQ("local", walker.resolve<string>("array", 0).c_str());
  // unset local
  walker.unset("array", 0);
  EXPECT_STREQ("", walker.resolve<string>("array", 0).c_str());
  // unset local
  walker.unset("array");
  // resolve to global
  EXPECT_STREQ("1", walker.resolve<string>("array", 0).c_str());
  EXPECT_STREQ("2", walker.resolve<string>("array", 1).c_str());
  EXPECT_STREQ("3", walker.resolve<string>("array", 2).c_str());
  // unset global
  walker.unset("array");
  EXPECT_STREQ("", walker.resolve<string>("array", 0).c_str());
  EXPECT_STREQ("", walker.resolve<string>("array", 1).c_str());
  EXPECT_STREQ("", walker.resolve<string>("array", 2).c_str());
  walker.unset("array");

  EXPECT_THROW(walker.unset("ro_array", 1), unset_exception);
  EXPECT_THROW(walker.unset("ro_local_array", 1), unset_exception);
  EXPECT_THROW(walker.unset("ro_array"), unset_exception);
  EXPECT_THROW(walker.unset("ro_local_array"), unset_exception);

  EXPECT_THROW(walker.unset("1", 1), libbash::interpreter_exception);
}

TEST(interpreter, unset_variables)
{
  interpreter walker;
  walker.define("var", "123");
  walker.define("ro_var", "123", true);
  interpreter::local_scope temp_scope(walker);
  walker.define_local("var", 456);
  walker.define_local("ro_local_var", 456, true);

  EXPECT_STREQ("456", walker.resolve<string>("var").c_str());
  walker.unset("var");
  EXPECT_STREQ("123", walker.resolve<string>("var").c_str());
  walker.unset("var");
  EXPECT_STREQ("", walker.resolve<string>("var").c_str());
  walker.unset("var");

  EXPECT_THROW(walker.unset("ro_var"), unset_exception);
  EXPECT_THROW(walker.unset("ro_local_var"), unset_exception);
  EXPECT_THROW(walker.unset("1"), libbash::interpreter_exception);
}

TEST(interpreter, unset_functions)
{
  interpreter walker;
  walker.push_current_ast(0);
  walker.define_function("foo", 0);
  EXPECT_TRUE(walker.has_function("foo"));
  walker.unset_function("foo");
  EXPECT_FALSE(walker.has_function("foo"));
}

TEST(interperter, substring_expansion)
{
  interpreter walker;
  EXPECT_STREQ("", walker.do_substring_expansion("@", 0, 1, 2).c_str());
  EXPECT_THROW(walker.do_substring_expansion("", 0, -1, 0), libbash::interpreter_exception);
}

TEST(interpreter, word_split)
{
  interpreter walker;
  std::vector<std::string> splitted_values;
  walker.split_word(" \n\t", splitted_values);
  EXPECT_EQ(0, splitted_values.size());

  splitted_values.clear();
  walker.split_word(" \tfoo\n bar \n", splitted_values);
  EXPECT_EQ(2, splitted_values.size());
  EXPECT_STREQ("foo", splitted_values[0].c_str());
  EXPECT_STREQ("bar", splitted_values[1].c_str());
}

TEST(interpreter, bash_additional_option)
{
  interpreter walker;

  EXPECT_THROW(walker.set_additional_option("not exist", false), libbash::interpreter_exception);
  EXPECT_THROW(walker.get_additional_option("not exist"), libbash::interpreter_exception);

  EXPECT_FALSE(walker.get_additional_option("extglob"));
  walker.set_additional_option("extglob", true);
  EXPECT_TRUE(walker.get_additional_option("extglob"));
}

TEST(interpreter, bash_option)
{
  interpreter walker;

  EXPECT_STREQ("Bh", walker.resolve<std::string>("-").c_str());
}
