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
/// \file printf_builtin.h
/// \brief implementation for the printf builtin
///

#include "builtins/printf_builtin.h"

#include <boost/format.hpp>

#include "core/interpreter.h"
#include "cppbash_builtin.h"

int printf_builtin::exec(const std::vector<std::string>& bash_args)
{
  std::vector<std::string>::const_iterator begin;
  if(!(bash_args[0] == "-v"))
    begin = bash_args.begin();
  else if(bash_args.size() < 3)
    throw libbash::illegal_argument_exception("printf: illegal number of arguments");
  else
    begin = bash_args.begin() + 2;

  std::stringstream format_string;
  transform_escapes(*begin, format_string);
  boost::format formatter(format_string.str());
  for(auto iter = begin + 1; iter != bash_args.end(); ++iter)
    formatter = formatter % *iter;

  if(!(bash_args[0][0] == '-'))
  {
    *_out_stream << formatter;
  }
  else if(bash_args[0] == "-v")
  {
    std::stringstream output;
    output << formatter;
    _walker.set_value(bash_args[1], output.str());
  }
  else
  {
    throw libbash::illegal_argument_exception("printf: invalid option: " + bash_args[0]);
  }

  return 0;
}
