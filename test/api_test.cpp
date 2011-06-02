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
/// \brief series of unit tests for the public interface
///

#include <gtest/gtest.h>

#include "libbash.h"
#include "test.h"

TEST(libbashapi, bad_path)
{
  std::unordered_map<std::string, std::vector<std::string>> variables;
  std::vector<std::string> functions;
  EXPECT_THROW(libbash::interpret("not exist", variables, functions),
               interpreter_exception);
  EXPECT_THROW(libbash::interpret("/scripts/source_true.sh", "not exist", variables, functions),
               interpreter_exception);
}

TEST(libbashapi, illegal_script)
{
  std::unordered_map<std::string, std::vector<std::string>> variables;
  std::vector<std::string> functions;
  EXPECT_THROW(libbash::interpret(get_src_dir() + "/scripts/illegal_script.sh", variables, functions),
               interpreter_exception);
}

TEST(libbashapi, legal_script)
{
  std::unordered_map<std::string, std::vector<std::string>> variables;
  std::vector<std::string> functions;
  int result = libbash::interpret(get_src_dir() + std::string("/scripts/source_true.sh"),
                                  variables,
                                  functions);
  EXPECT_EQ(0, result);

  result = libbash::interpret(get_src_dir() + std::string("/scripts/source_false.sh"),
                              variables,
                              functions);
  EXPECT_NE(0, result);

  EXPECT_STREQ((get_src_dir() + std::string("/scripts/source_false.sh")).c_str(),
                variables["0"][0].c_str());
}

TEST(libbashapi, preload)
{
  std::unordered_map<std::string, std::vector<std::string>> variables;
  std::vector<std::string> functions;
  int result = libbash::interpret(get_src_dir() + std::string("/scripts/source_true.sh"),
                                  get_src_dir() + std::string("/scripts/source_true.sh"),
                                  variables,
                                  functions);
  EXPECT_EQ(0, result);
  result = libbash::interpret(get_src_dir() + std::string("/scripts/source_true.sh"),
                              get_src_dir() + std::string("/scripts/source_false.sh"),
                              variables,
                              functions);
  EXPECT_NE(0, result);

  EXPECT_THROW(libbash::interpret(get_src_dir() + std::string("/scripts/source_true.sh"),
                                  get_src_dir() + std::string("/scripts/illegal_script.sh"),
                                  variables,
                                  functions),
               interpreter_exception);
}
