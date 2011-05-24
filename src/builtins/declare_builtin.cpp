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
/// \file declare_builtin.cpp
/// \author Mu Qiao
/// \brief class that implements the declare builtin
///
#include <iostream>

#include "builtins/declare_builtin.h"

int declare_builtin::exec(const std::vector<std::string>& bash_args)
{
  if(bash_args.empty())
  {
    *_err_stream << "Arguments required for declare" << std::endl;
    return 1;
  }
  else if(bash_args[0].size() != 2)
  {
    *_err_stream << "Multiple arguments are not supported" << std::endl;
    return 1;
  }

  if(bash_args[0][0] != '-' && bash_args[0][0] != '+')
  {
    *_err_stream << "Invalid option for declare builtin" << std::endl;
    return 1;
  }

  switch(bash_args[0][1])
  {
    case 'a':
    case 'A':
    case 'f':
    case 'F':
    case 'i':
    case 'l':
    case 'r':
    case 't':
    case 'u':
    case 'x':
    case 'p':
      *_err_stream << "declare " << bash_args[0] << " is not supported yet" << std::endl;
      return 1;
    default:
      *_err_stream << "Unrecognized option for declare: " << bash_args[0] << std::endl;
      return 1;
  }
}
