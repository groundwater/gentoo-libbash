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
/// \file shift_tests.cpp
/// \brief series of unit tests for shift builtin
///
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

#include "core/exceptions.h"
#include "core/interpreter.h"
#include "cppbash_builtin.h"

TEST(shift_builtin_test, bad_argument)
{
  interpreter walker;
  std::map<unsigned, std::string> values = {{1, "1"}, {2, "2"}, {3, "3"}};
  walker.define("*", values);

  EXPECT_NE(0, cppbash_builtin::exec("shift", {"-1"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_NE(0, cppbash_builtin::exec("shift", {"4"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_THROW(cppbash_builtin::exec("shift", {"1", "2"}, std::cout, std::cerr, std::cin, walker),
               libbash::illegal_argument_exception);
  EXPECT_THROW(cppbash_builtin::exec("shift", {"abc"}, std::cout, std::cerr, std::cin, walker),
               boost::bad_lexical_cast);
}

TEST(shift_builtin_test, shift_all)
{
  interpreter walker;
  std::map<unsigned, std::string> values = {{1, "1"}, {2, "2"}, {3, "3"}};
  walker.define("*", values);

  EXPECT_EQ(0, cppbash_builtin::exec("shift", {"3"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_EQ(0, walker.get_array_length("*"));
}

TEST(shift_builtin_test, normal)
{
  interpreter walker;
  std::map<unsigned, std::string> values = {{1, "1"}, {2, "2"}, {3, "3"}};
  walker.define("*", values);

  EXPECT_EQ(0, cppbash_builtin::exec("shift", {"2"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_EQ(1, walker.get_array_length("*"));
  EXPECT_STREQ("3", walker.resolve<std::string>("*", 1).c_str());

  walker.define("*", values);
  EXPECT_EQ(0, cppbash_builtin::exec("shift", {"1"}, std::cout, std::cerr, std::cin, walker));
  EXPECT_EQ(2, walker.get_array_length("*"));
  EXPECT_STREQ("2", walker.resolve<std::string>("*", 1).c_str());
  EXPECT_STREQ("3", walker.resolve<std::string>("*", 2).c_str());
}
