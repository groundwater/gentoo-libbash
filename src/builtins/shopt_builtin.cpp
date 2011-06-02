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
/// \file shopt_builtin.cpp
/// \brief implementation for the shopt builtin
///

#include "builtins/shopt_builtin.h"

#include "core/interpreter.h"
#include "core/interpreter_exception.h"
#include "cppbash_builtin.h"

void shopt_builtin::set_opt(const std::vector<std::string>& bash_args, bool value)
{
  for(auto iter = bash_args.begin() + 1; iter != bash_args.end(); ++iter)
      _walker.set_option(*iter, value);
}

int shopt_builtin::exec(const std::vector<std::string>& bash_args)
{
  if(bash_args.empty())
    throw interpreter_exception("Arguments required for shopt");
  else if(bash_args[0].size() != 2)
    throw interpreter_exception("Multiple arguments are not supported");

  switch(bash_args[0][1])
  {
    case 'u':
      set_opt(bash_args, false);
      break;
    case 's':
      set_opt(bash_args, true);
      break;
    case 'q':
    case 'o':
      throw interpreter_exception("shopt " + bash_args[0] + " is not supported yet");
    default:
      throw interpreter_exception("Unrecognized option for shopt: " + bash_args[0]);
  }

  return 0;
}
