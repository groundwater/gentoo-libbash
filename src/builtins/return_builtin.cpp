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
/// \file return_builtin.h
/// \brief implementation for the return builtin
///

#include "builtins/return_builtin.h"

#include "builtins/builtin_exceptions.h"
#include "core/interpreter.h"
#include "cppbash_builtin.h"

int return_builtin::exec(const std::vector<std::string>& bash_args)
{
  if(bash_args.size() > 1)
    throw interpreter_exception("return: too many arguments");
  else if(bash_args.size() == 1)
    _walker.set_status(boost::lexical_cast<int>(bash_args[0]));

  throw return_exception();
}
