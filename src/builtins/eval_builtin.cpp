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
/// \file eval_builtin.h
/// \author Mu Qiao
/// \brief class that implements the eval builtin
///

#include "builtins/eval_builtin.h"

#include <sstream>

#include <boost/algorithm/string/join.hpp>

#include "core/bash_ast.h"
#include "core/interpreter.h"

int eval_builtin::exec(const std::vector<std::string>& bash_args)
{
  std::stringstream script(boost::algorithm::join(bash_args, " "));
  bash_ast(script).interpret_with(_walker);
  return _walker.get_status();
}
