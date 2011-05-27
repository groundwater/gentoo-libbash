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
/// \file bash_ast_test.cpp
/// \author Mu Qiao
/// \brief series of unit tests for interpreter.
///

#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "core/bash_ast.h"
#include "core/interpreter.h"
#include "test.h"

TEST(bash_ast, parse_illegal_script)
{
  bash_ast ast(get_src_dir() + std::string("/scripts/illegal_script.sh"));
  EXPECT_NE(0, ast.get_error_count());
}

TEST(bash_ast, parse_legal_script)
{
  bash_ast ast(get_src_dir() + std::string("/scripts/source_true.sh"));
  EXPECT_EQ(0, ast.get_error_count());

  bash_ast ast2(get_src_dir() + std::string("/scripts/source_false.sh"));
  EXPECT_EQ(0, ast2.get_error_count());
}

TEST(bash_ast, parse_arithmetics)
{
  std::string expr("1 + 2");
  bash_ast ast(std::stringstream(expr), bash_ast::parser_arithmetics);
  interpreter walker;
  EXPECT_EQ(3, ast.interpret_with(walker, &bash_ast::walker_arithmetics));
}

TEST(bash_ast, illegal_path)
{
  EXPECT_THROW(bash_ast("not_exist"), interpreter_exception);
}
