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
/// \file unset_tests.cpp
/// \brief series of unit tests for unset builtin
///
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

#include "core/interpreter.h"
#include "cppbash_builtin.h"
#include "exceptions.h"

TEST(unset_builtin_test, normal)
{
  interpreter walker;

  walker.set_value("var", "foo");
  EXPECT_EQ(0, cppbash_builtin::exec("unset", {"var"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_STREQ("", walker.resolve<std::string>("var").c_str());
}

TEST(unset_builtin_test, array)
{
  interpreter walker;

  walker.set_value("array", "foo bar", 2);
  EXPECT_EQ(0, cppbash_builtin::exec("unset", {"array"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_STREQ("", walker.resolve<std::string>("array", 2).c_str());

  walker.set_value("array", "foo bar", 2);
  EXPECT_EQ(0, cppbash_builtin::exec("unset", {"array[2]"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_STREQ("", walker.resolve<std::string>("array", 2).c_str());
}
