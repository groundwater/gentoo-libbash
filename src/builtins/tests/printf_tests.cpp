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
/// \file printf_tests.cpp
/// \brief series of unit tests for printf builtin
///
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

#include "builtins/builtin_exceptions.h"
#include "core/interpreter.h"
#include "cppbash_builtin.h"

namespace
{
  void verify_error(const std::vector<std::string>& arguments, const std::string& expected, interpreter& walker)
  {
    try
    {
      cppbash_builtin::exec("printf", arguments, std::cout, std::cerr, std::cin, walker);
      FAIL();
    }
    catch(libbash::interpreter_exception& e)
    {
      EXPECT_STREQ(expected.c_str(), e.what());
    }
  }

  void verify_output(const std::vector<std::string>& arguments, const std::string& expected, interpreter& walker)
  {
    std::stringstream output;
    EXPECT_EQ(0, cppbash_builtin::exec("printf", arguments, output, std::cerr, std::cin, walker));
    EXPECT_STREQ(expected.c_str(), output.str().c_str());
  }
}

TEST(printf_builtin_test, bad_argument)
{
  interpreter walker;
  verify_error({"-v"}, "printf: illegal number of arguments", walker);
  verify_error({"-p"}, "printf: invalid option: -p", walker);
}

TEST(printf_builtin_test, normal)
{
  interpreter walker;
  verify_output({"-v", "foo", "%s\n", "bar"}, "", walker);
  EXPECT_STREQ(walker.resolve<std::string>("foo").c_str(), "bar\n");

  verify_output({"%s %s\n", "foo", "bar"}, "foo bar\n", walker);
}
