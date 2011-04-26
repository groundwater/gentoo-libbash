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
/// \file libbash.cpp
/// \author Mu Qiao
/// \brief implementation for libbash interface
///

#include "libbash.h"

#include <fstream>

#include "core/interpreter.h"
#include "core/bash_ast.h"

namespace libbash
{
  void interpret(const std::string& path,
                 std::unordered_map<std::string, std::vector<std::string>>& variables,
                 std::vector<std::string>& functions)
  {
    std::ifstream input(path.c_str());
    if(!input)
      throw interpreter_exception("Unable to create fstream for script: " + path);
    interpreter walker;
    bash_ast ast(input);
    ast.interpret_with(walker);

    for(auto iter = walker.begin(); iter != walker.end(); ++iter)
      iter->second->get_all_values<std::string>(variables[iter->first]);
    walker.get_all_function_names(functions);
  }
}
