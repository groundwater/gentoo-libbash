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

TEST(interpreter, is_unset_or_null)
{
  interpreter walker;
  walker.define("foo", "hello");
  EXPECT_FALSE(walker.is_unset_or_null("foo"));
  walker.define("foo", "hello", false, true);
  EXPECT_TRUE(walker.is_unset_or_null("foo"));
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
