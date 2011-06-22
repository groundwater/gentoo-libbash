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
/// \file echo_builtin.cpp
/// \brief class that implements the echo builtin
///

#include "echo_builtin.h"
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include "builtins/builtin_exceptions.h"

namespace qi = boost::spirit::qi;
namespace karma = boost::spirit::karma;
namespace phoenix = boost::phoenix;

int echo_builtin::exec(const std::vector<std::string>& bash_args)
{
  bool suppress_nl = false;
  bool enable_escapes = false;
  bool options_parsed = false;

  if(bash_args.empty())
  {
    this->out_buffer() << std::endl;
    return 0;
  }

  for(auto i = bash_args.begin(); i != bash_args.end(); i++)
  {
    const std::string& str = *i;

    if(!options_parsed)
    {
      options_parsed = determine_options(str, suppress_nl, enable_escapes);
    }

    if(options_parsed)
    {
      if(enable_escapes)
      {
        for(; i != bash_args.end(); i++)
        {
          try
          {
            cppbash_builtin::transform_escapes(*i, out_buffer(), false);
          }
          catch(suppress_output)
          {
            return 0;
          }
        }
      }
      else
      {
        this->out_buffer() << karma::format(karma::string % ' ', std::vector<std::string>(i, bash_args.end()));
      }

      if(!suppress_nl)
        this->out_buffer() << std::endl;

      return 0;
    }
  }

  return 0;
}

bool echo_builtin::determine_options(const std::string &string, bool &suppress_nl, bool &enable_escapes)
{
  using phoenix::ref;
  using qi::char_;

  bool n_matched = false, e_matched = false, E_matched = false;

  auto options = '-' >
    +(
      char_('n')[ref(n_matched) = true] |
      char_('e')[ref(e_matched) = true, ref(E_matched) = false] |
      char_('E')[ref(E_matched) = true, ref(e_matched) = false]
    );

  auto first = string.begin();
  qi::parse(first, string.end(), options);

  if(first != string.end()) {
    return true;
  }
  else
  {
    if(n_matched)
      suppress_nl = true;

    if(e_matched)
      enable_escapes = true;

    if(E_matched)
      enable_escapes = false;

    return false;
  }
}
