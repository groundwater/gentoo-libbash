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
/// \file api_test.cpp
/// \author Mu Qiao
/// \brief series of unit tests for the public interface
///

#include <gtest/gtest.h>

#include "libbash.h"

using namespace std;

TEST(libbashapi, bad_path)
{
  std::unordered_map<std::string, std::vector<std::string>> variables;
  std::vector<std::string> functions;
  EXPECT_THROW(libbash::interpret("not exist", variables, functions),
               interpreter_exception);
}
