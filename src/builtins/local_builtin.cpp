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
/// \file local_builtin.h
/// \brief class that implements the local builtin
///

#include "builtins/local_builtin.h"

#include <functional>

#include <sstream>

#include "core/bash_ast.h"
#include "core/interpreter.h"

int local_builtin::exec(const std::vector<std::string>& bash_args)
{
  std::stringstream script;
  for(auto iter = bash_args.begin(); iter != bash_args.end(); ++iter)
      script << *iter;

  bash_ast ast(script, std::bind(&bash_ast::parser_builtin_variable_definitions, std::placeholders::_1, true));
  ast.interpret_with(_walker);

  return 0;
}
