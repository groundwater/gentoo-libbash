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
/// \file unset_builtin.h
/// \brief implementation for the unset builtin
///
#include "builtins/unset_builtin.h"

#include <functional>

#include "core/interpreter.h"

using namespace boost::xpressive;

int unset_builtin::exec(const std::vector<std::string>& bash_args)
{
  if(bash_args.empty())
    return 0;

  if(bash_args[0] == "-f")
    for_each(bash_args.begin() + 1,
             bash_args.end(),
             std::bind(&interpreter::unset_function, &_walker, std::placeholders::_1));
  else
  /* POSIX says if neither -f nor -v is specified, name refers to a variable;
   * if a variable by that name does not exist, it is unspecified whether a
   * function by that name, if any, shall be unset.
   *
   * >=bash-4.1: without options, unset first tries to unset a variable, and
   * if that fails, tries to unset a function.
   * (We haven't checked bash-4.0)
   *
   * bash-3.2: if no options are supplied, or the -v option is given, each
   * name refers to a shell variable.
   *
   * We addhere to bash-3.2
   * */
    for_each(bash_args.front() == "-v" ? bash_args.begin() + 1 : bash_args.begin(),
             bash_args.end(),
             [&](const std::string& name) {
                static const sregex index_pattern = sregex::compile("^(.*)\\[(\\d*)\\]$");
                smatch match;
                if(regex_match(name, match, index_pattern))
                  _walker.unset(match[1], boost::lexical_cast<unsigned int>(match[2]));
                else
                  _walker.unset(name);
             });

  return 0;
}
