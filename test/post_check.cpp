/*
Copyright 2011 Mu Qiao

This file is part of libbash.

libbash is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

libbash is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libbash.  If not, see <http://www.gnu.org/licenses/>.
*/
///
/// \file post_check.cpp
/// \brief series of unit tests for generated source files.
/// \author Mu Qiao
///

#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

using namespace std;

static void check_file_size(const char *path, uintmax_t size_max)
{
  uintmax_t file_size = boost::filesystem::file_size(path);
  EXPECT_GT(file_size, 0);
  EXPECT_LT(file_size, size_max);
}

TEST(post_check, lexer_size)
{
  check_file_size("bashastLexer.c", 1024 * 1024);
}

TEST(post_check, parser_size)
{
  check_file_size("bashastParser.c", 4096 * 1024);
}
