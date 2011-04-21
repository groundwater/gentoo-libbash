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
#include <string>

#include "cppbash_builtin.h"
#include "core/interpreter.h"
#include "core/interpreter_exception.h"
#include "core/bash_ast.h"

int source_builtin::exec(const std::vector<std::string>& bash_args)
{
  if(bash_args.size() == 0)
    throw interpreter_exception("should provide one argument for source builtin");

  // we need fix this to pass extra arguments as positional parameters
  const std::string& path = bash_args[0];
  std::ifstream input(path);
  if(!input)
    throw interpreter_exception(path + " can't be read");

  bash_ast ast(input);
  ast.interpret_with(_walker);

  return _walker.get_status();
}
