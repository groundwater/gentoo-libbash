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
/// \file metadata_generator.cpp
/// \author Mu Qiao
/// \brief a simple utility for generating metadata
///
#include <iostream>
#include <string>
#include <vector>

#include "libbash.h"

static std::vector<std::string> metadata_names = {"DEPEND", "RDEPEND", "SLOT", "SRC_URI",
                                                  "RESTRICT",  "HOMEPAGE",  "LICENSE", "DESCRIPTION",
                                                  "KEYWORDS",  "INHERITED", "IUSE", "REQUIRED_USE",
                                                  "PDEPEND",   "PROVIDE", "EAPI", "PROPERTIES", "DEFINED_PHASES"};

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    std::cerr<<"Please provide your script as an argument"<<std::endl;
    exit(EXIT_FAILURE);
  }

  std::unordered_map<std::string, std::vector<std::string>> variables;
  libbash::interpret(argv[1], variables);

  for(auto iter_name = metadata_names.begin(); iter_name != metadata_names.end(); ++iter_name)
  {
    auto iter_value = variables.find(*iter_name);
    if(iter_value != variables.end())
      std::cout << iter_value->second[0] << std::endl;
    else
      std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}
