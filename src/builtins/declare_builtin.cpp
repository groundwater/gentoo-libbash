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
/// \brief class that implements the declare builtin
///
#include "builtins/declare_builtin.h"

#include <algorithm>
#include <iostream>

#include "core/interpreter.h"

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

  int result = 0;
  switch(bash_args[0][1])
  {
    case 'F':
      if(bash_args[0][0] == '+')
        return 0;
      if(bash_args.size() > 1)
      {
        for(auto iter = bash_args.begin() + 1; iter != bash_args.end(); ++iter)
        {
          if(_walker.has_function(*iter))
            *_out_stream << *iter << std::endl;
          else
            result = 1;
        }
      }
      else
      {
        std::vector<std::string> functions;

        _walker.get_all_function_names(functions);
        sort(functions.begin(), functions.end());

        for(auto iter = functions.begin(); iter != functions.end(); ++iter)
          *_out_stream << "declare -f " << *iter << std::endl;
      }
      return result;
    case 'p':
      if(bash_args.size() > 1)
      {
        for(auto iter = bash_args.begin() + 1; iter != bash_args.end(); ++iter)
        {
          // We do not print the type of the variable for now
          if(!_walker.is_unset(*iter))
          {
            *_out_stream << "declare -- " << *iter << "=\"" << _walker.resolve<std::string>(*iter) << "\"" << std::endl;
          }
          else
          {
            *_out_stream << "-bash: declare: " << *iter << ": not found" << std::endl;
            result = 1;
          }
        }
      }
      else
      {
        throw interpreter_exception("We do not support declare -p without arguments for now");
      }
      return result;
    case 'a':
    case 'A':
    case 'f':
    case 'i':
    case 'l':
    case 'r':
    case 't':
    case 'u':
    case 'x':
      *_err_stream << "declare " << bash_args[0] << " is not supported yet" << std::endl;
      return 1;
    default:
      *_err_stream << "Unrecognized option for declare: " << bash_args[0] << std::endl;
      return 1;
  }
}
