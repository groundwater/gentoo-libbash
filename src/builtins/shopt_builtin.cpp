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
/// \author Mu Qiao
/// \brief implementation for the shopt builtin
///

#include "core/interpreter.h"
#include "core/interpreter_exception.h"
#include "cppbash_builtin.h"

#include "builtins/shopt_builtin.h"

int shopt_builtin::set_opt(const std::vector<std::string>& bash_args, bool value)
{
  int result = 0;
  for(auto iter = bash_args.begin() + 1; iter != bash_args.end(); ++iter)
  {
    try
    {
      _walker.set_option(*iter, value);
    }
    catch(interpreter_exception& e)
    {
      std::cerr << *iter << " is not a valid bash option" << std::endl;
      result = 1;
    }
  }
  return result;
}

int shopt_builtin::exec(const std::vector<std::string>& bash_args)
{
  if(bash_args.empty())
  {
    *_err_stream << "Arguments required for shopt" << std::endl;
    return 1;
  }
  else if(bash_args[0].size() != 2)
  {
    *_err_stream << "Multiple arguments are not supported" << std::endl;
    return 1;
  }

  switch(bash_args[0][1])
  {
    case 'u':
      return set_opt(bash_args, false);
    case 's':
      return set_opt(bash_args, true);
    case 'q':
    case 'o':
      *_err_stream << "shopt " << bash_args[0] << " is not supported yet" << std::endl;
      return 1;
    default:
      *_err_stream << "Unrecognized option for shopt: " << bash_args[0] << std::endl;
      return 1;
  }
}
