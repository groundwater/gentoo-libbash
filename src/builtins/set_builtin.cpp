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
/// \file set_builtin.cpp
/// \brief class that implements the set builtin
///
#include "builtins/set_builtin.h"

#include "core/interpreter.h"
#include "exceptions.h"

int set_builtin::exec(const std::vector<std::string>& bash_args)
{
  if(bash_args.empty())
  {
    throw libbash::unsupported_exception("set: variables printing are not supported");
    return 1;
  }

  if(bash_args[0][0] != '-' && bash_args[0][0] != '+')
  {
    throw libbash::illegal_argument_exception("set: invalid option");
    return 1;
  }

  switch(bash_args[0][1])
  {
    case '-':
      if(bash_args[0][0] != '-') {
        throw libbash::unsupported_exception("set: invalid option");
        return 1;
      }
      else
      {
        _walker.define_positional_arguments(bash_args.begin() + 1, bash_args.end());
        return 0;
      }
    case 'a':
    case 'b':
    case 'e':
    case 'f':
    case 'h':
    case 'k':
    case 'm':
    case 'n':
    case 'p':
    case 't':
    case 'u':
      _walker.set_option('u', bash_args[0][0] == '-');
      return 0;
    case 'v':
    case 'x':
    case 'B':
    case 'C':
    case 'E':
    case 'H':
    case 'P':
    case 'T':
      throw libbash::unsupported_exception("set " + bash_args[0] + " is not supported yet");
      return 1;
    default:
      throw libbash::illegal_argument_exception("set: unrecognized option: " + bash_args[0]);
      return 1;
  }
}
