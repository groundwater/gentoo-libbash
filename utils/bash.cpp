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
/// \file bash_runner.cpp
/// \brief a program to write variables in a script into standard output
///

#include <algorithm>
#include <iostream>
#include <map>

#include <gtest/gtest.h>
#include <boost/spirit/include/karma.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include "libbash.h"
#include "test.h"

static const std::vector<std::string> special_variables
{
  "IFS", "*", "0", "-"
};

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    std::cerr<<"Please provide your script as an argument"<<std::endl;
    exit(EXIT_FAILURE);
  }

  std::unordered_map<std::string, std::vector<std::string>> variables{
    {"srcdir", std::vector<std::string>({get_src_dir()})}
  };
  std::vector<std::string> functions;
  try
  {
    libbash::interpret(argv[1], variables, functions);
  }
  catch(libbash::interpreter_exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return 0;
}
