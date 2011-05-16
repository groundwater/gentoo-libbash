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
/// \file source_builtin.h
/// \author Mu Qiao
/// \brief class that implements the source builtin
///

#include "builtins/source_builtin.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "builtins/builtin_exceptions.h"
#include "cppbash_builtin.h"
#include "core/interpreter.h"
#include "core/interpreter_exception.h"
#include "core/bash_ast.h"

int source_builtin::exec(const std::vector<std::string>& bash_args)
{
  static std::unordered_map<std::string, std::shared_ptr<bash_ast>> ast_cache;

  if(bash_args.size() == 0)
    throw interpreter_exception("should provide one argument for source builtin");

  // we need fix this to pass extra arguments as positional parameters
  const std::string& path = bash_args[0];

  auto& stored_ast = ast_cache[path];
  if(!stored_ast)
  {
    std::ifstream input(path);
    if(!input)
      throw interpreter_exception(path + " can't be read");

    stored_ast.reset(new bash_ast(input));
    if(stored_ast->get_error_count())
      std::cerr << path << " could not be parsed properly" << std::endl;
  }

  try
  {
    stored_ast->interpret_with(_walker);
  }
  catch(return_exception& e) {}

  return _walker.get_status();
}