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
/// \file source_tests.cpp
/// \brief series of unit tests for source built in
///

#include <cstdlib>

#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "core/interpreter.h"
#include "cppbash_builtin.h"

using namespace std;

TEST(source_builtin_test, source)
{
  std::string srcdir(getenv("srcdir"));
  interpreter walker;
  int status = cppbash_builtin::exec("source", {srcdir + "/scripts/source_true.sh"}, std::cout, std::cerr, std::cin, walker);
  EXPECT_EQ(status, 0);
  EXPECT_TRUE(walker.has_function("foo"));
  EXPECT_STREQ("hello", walker.resolve<std::string>("FOO001").c_str());

  status = cppbash_builtin::exec("source", {srcdir + "/scripts/source_false.sh"}, std::cout, std::cerr, std::cin, walker);
  EXPECT_EQ(status, 1);
}
