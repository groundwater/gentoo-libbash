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

#include "core/interpreter_exception.h"
#include "cppbash_builtin.h"

#include "builtins/shopt_builtin.h"

namespace
{
  int disable_opt(const std::vector<std::string>& bash_args)
  {
    auto iter = find(bash_args.begin() + 1, bash_args.end(), "extglob");
    if(iter != bash_args.end())
      throw interpreter_exception("Disabling extglob is not allowed");
    return 0;
  }
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
      return disable_opt(bash_args);
    case 's':
    case 'q':
    case 'o':
      *_err_stream << "shopt " << bash_args[0] << " is not supported yet" << std::endl;
      return 1;
    default:
      *_err_stream << "Unrecognized option for shopt: " << bash_args[0] << std::endl;
      return 1;
  }
}
