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

namespace internal
{
  int interpret(interpreter& walker,
                const std::ifstream& input,
                std::unordered_map<std::string, std::vector<std::string>>& variables,
                std::vector<std::string>& functions)
  {
    int result = 0;

    // Initialize bash environment
    for(auto iter = variables.begin(); iter != variables.end(); ++iter)
      walker.set_value(iter->first, (iter->second)[0]);

    bash_ast ast(input);
    ast.interpret_with(walker);
    result += ast.get_error_count();

    for(auto iter = walker.begin(); iter != walker.end(); ++iter)
      iter->second->get_all_values<std::string>(variables[iter->first]);
    walker.get_all_function_names(functions);

    result += walker.get_status();
    return result;
  }
}

namespace libbash
{
  int interpret(const std::string& target_path,
                std::unordered_map<std::string, std::vector<std::string>>& variables,
                std::vector<std::string>& functions)
  {
    std::ifstream input(target_path.c_str());
    if(!input)
      throw interpreter_exception("Unable to create fstream for script: " + target_path);

    interpreter walker;

    return internal::interpret(walker, input, variables, functions);
  }

  int interpret(const std::string& target_path,
                const std::string& preload_path,
                std::unordered_map<std::string, std::vector<std::string>>& variables,
                std::vector<std::string>& functions)
  {
    std::ifstream input(target_path.c_str());
    if(!input)
      throw interpreter_exception("Unable to create fstream for script: " + target_path);

    std::ifstream preload(preload_path.c_str());
    if(!preload)
      throw interpreter_exception("Unable to create fstream for script: " + preload_path);

    interpreter walker;

    // Preloading
    bash_ast preload_ast(preload);
    preload_ast.interpret_with(walker);
    int result = preload_ast.get_error_count();
    if(result)
    {
      std::cerr << "Error occured while preloading" << std::endl;
      return result;
    }

    return internal::interpret(walker, input, variables, functions);
  }
}
