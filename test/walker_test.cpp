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
/// \file lib_test.cpp
/// \author Mu Qiao
/// \brief series of unit tests for generated walker.
///

#include <sstream>

#include <gtest/gtest.h>

#include "core/interpreter.h"
#include "core/parser_builder.h"
#include "core/walker_builder.h"

static void check_string_assignment(const char* script,
                                    const std::string& name,
                                    const char* exp_value)
{
  std::istringstream input(script);
  parser_builder pbuilder(input);
  walker_builder wbuilder = pbuilder.create_walker_builder();
  EXPECT_STREQ(exp_value, wbuilder.walker->resolve<std::string>(name).c_str());
}

#define TEST_STRING_ASSIGNMENT(name, script, var_name, exp_value)\
  TEST(string_assignment, name) \
  { check_string_assignment(script, var_name, exp_value); }

TEST_STRING_ASSIGNMENT(str_assignment,          "str=\"abc\"",          "str",    "abc")
TEST_STRING_ASSIGNMENT(str_assignment2,         "str=\"abc_def\"",      "str",    "abc_def")
TEST_STRING_ASSIGNMENT(str_assignment3,         "str=\"abc def\"",      "str",    "abc def")
TEST_STRING_ASSIGNMENT(str_assignment4,
                       "str=\"case esac do done elif else fi if for function in select then until while time\"",
                       "str",
                       "case esac do done elif else fi if for function in select then until while time")
TEST_STRING_ASSIGNMENT(str_assignment5,
                       "str=\"123 abc = % %% - . .. :  -a -aa test _ ~ ++ -- *= /= %= += -= <<= >>= &= ^= |= \\a ^ aä\"",
                       "str",
                       "123 abc = % %% - . .. :  -a -aa test _ ~ ++ -- *= /= %= += -= <<= >>= &= ^= |= \\a ^ aä")
TEST_STRING_ASSIGNMENT(str_assignment6,
                       "str=\"/ \n \r\n & && ||| || > < ' : ; , ( (( ) )) ;; { } >= <=\"",
                       "str",
                       "/ \n \r\n & && ||| || > < ' : ; , ( (( ) )) ;; { } >= <=")

TEST(array_index, out_of_bound)
{
  std::string script = "a[-1]=\"1\"";
  std::istringstream input(script);
  parser_builder pbuilder(input);
  EXPECT_THROW(pbuilder.create_walker_builder(), interpreter_exception);

  std::string script2 = "a=(1 2 [-5]=1)";
  std::istringstream input2(script2);
  parser_builder pbuilder2(input2);
  EXPECT_THROW(pbuilder2.create_walker_builder(), interpreter_exception);
}
