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
/// \file set_tests.cpp
/// \brief series of unit tests for set builtin
///
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

#include "core/interpreter.h"
#include "cppbash_builtin.h"
#include "exceptions.h"

TEST(set_builtin_test, positional)
{
  interpreter walker;

  EXPECT_EQ(0, cppbash_builtin::exec("set", {"--", "1", "2", "3"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_EQ(3, walker.get_array_length("*"));
  EXPECT_STREQ("1", walker.resolve<std::string>("*", 1).c_str());
  EXPECT_STREQ("2", walker.resolve<std::string>("*", 2).c_str());
  EXPECT_STREQ("3", walker.resolve<std::string>("*", 3).c_str());

  EXPECT_EQ(0, cppbash_builtin::exec("set", {"--"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_EQ(0, walker.get_array_length("*"));
  EXPECT_STREQ("", walker.resolve<std::string>("*", 1).c_str());
}

TEST(set_builtin_test, u_option)
{
  interpreter walker;

  EXPECT_EQ(0, cppbash_builtin::exec("set", {"-u"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_THROW(walker.resolve<std::string>("VAR1").c_str(), libbash::unsupported_exception);

  walker.set_value("ARRAY", "foo");
  EXPECT_NO_THROW(walker.resolve<std::string>("ARRAY").c_str());
  EXPECT_THROW(walker.resolve<std::string>("ARRAY", 2).c_str(), libbash::unsupported_exception);

  walker.set_value("ARRAY", "foo", 3);
  EXPECT_NO_THROW(walker.resolve<std::string>("ARRAY", 3).c_str());

  EXPECT_EQ(0, cppbash_builtin::exec("set", {"+u"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_NO_THROW(walker.resolve<std::string>("VAR2", 1).c_str());

}
