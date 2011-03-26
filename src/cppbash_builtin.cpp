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
/// \file cppbash_builtin.cpp
/// \author Nathan Eloe
/// \brief Implementation of class to inherit builtins from
///

#include "cppbash_builtin.h"
#include "builtins/echo_builtin.h"
#include "builtins/boolean_builtins.h"

cppbash_builtin::cppbash_builtin(std::ostream &outstream, std::ostream &errstream, std::istream &instream): _out_stream(&outstream), _err_stream(&errstream), _inp_stream(&instream)
{
}

cppbash_builtin::builtins_type& cppbash_builtin::builtins() {
  static boost::scoped_ptr<builtins_type> p(new builtins_type {
      {"echo", boost::factory<echo_builtin*>()},
      {"true", boost::factory<true_builtin*>()},
      {"false", boost::factory<false_builtin*>()}
  });
  return *p;
}
