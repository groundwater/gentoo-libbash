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

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "core/bash_ast.h"
#include "core/interpreter.h"
#include "exceptions.h"

int declare_builtin::exec(const std::vector<std::string>& bash_args)
{
  if(bash_args.empty())
  {
    throw libbash::illegal_argument_exception("declare: arguments required");
    return 1;
  }

  int result = 0;

  std::vector<std::string> tokens;
  boost::split(tokens, bash_args[0], boost::is_any_of(" "));

  if(tokens[0][0] == '-' || tokens[0][0] == '+')
  {
    if(tokens[0].size() > 2)
      throw libbash::unsupported_exception("declare: " + tokens[0] + " is not supported yet");

    switch(tokens[0][1])
    {
      case 'F':
        if(tokens[0][0] == '+')
          return 0;
        if(tokens.size() > 1)
        {
          for(auto iter = tokens.begin() + 1; iter != tokens.end(); ++iter)
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
        if(tokens.size() > 1)
        {
          for(auto iter = tokens.begin() + 1; iter != tokens.end(); ++iter)
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
          throw libbash::unsupported_exception("We do not support declare -p without arguments for now");
        }
        return result;
      case 'a':
      case 'i':
      case 'A':
      case 'f':
      case 'l':
      case 'r':
      case 't':
      case 'u':
      case 'x':
        throw libbash::unsupported_exception("declare " + tokens[0] + " is not supported yet");
        return 1;
      default:
        throw libbash::illegal_argument_exception("declare: unrecognized option: " + tokens[0]);
        return 1;
    }
  }

  std::stringstream script;
  for(auto iter = bash_args.begin(); iter != bash_args.end(); ++iter)
      script << *iter + " ";

  bash_ast ast(script, std::bind(&bash_ast::parser_builtin_variable_definitions, std::placeholders::_1, false));
  ast.interpret_with(_walker);

  return result;
}
