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
/// \brief Implementation of class to inherit builtins from
///

#include "cppbash_builtin.h"

#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include "builtins/boolean_builtins.h"
#include "builtins/builtin_exceptions.h"
#include "builtins/break_builtin.h"
#include "builtins/continue_builtin.h"
#include "builtins/declare_builtin.h"
#include "builtins/echo_builtin.h"
#include "builtins/eval_builtin.h"
#include "builtins/export_builtin.h"
#include "builtins/local_builtin.h"
#include "builtins/inherit_builtin.h"
#include "builtins/let_builtin.h"
#include "builtins/return_builtin.h"
#include "builtins/printf_builtin.h"
#include "builtins/shift_builtin.h"
#include "builtins/shopt_builtin.h"
#include "builtins/source_builtin.h"
#include "builtins/unset_builtin.h"
#include "builtins/read_builtin.h"
#include "builtins/set_builtin.h"
#include "builtins/unset_builtin.h"

namespace qi = boost::spirit::qi;
namespace karma = boost::spirit::karma;
namespace phoenix = boost::phoenix;

cppbash_builtin::cppbash_builtin(BUILTIN_ARGS): _out_stream(&out), _err_stream(&err), _inp_stream(&in), _walker(walker)
{
}

cppbash_builtin::builtins_type& cppbash_builtin::builtins() {
  static boost::scoped_ptr<builtins_type> p(new builtins_type {
      {"break", boost::factory<break_builtin*>()},
      {"continue", boost::factory<continue_builtin*>()},
      {"echo", boost::factory<echo_builtin*>()},
      {"eval", boost::factory<eval_builtin*>()},
      {"export", boost::factory<export_builtin*>()},
      {"local", boost::factory<local_builtin*>()},
      {"declare", boost::factory<declare_builtin*>()},
      {"source", boost::factory<source_builtin*>()},
      {"shift", boost::factory<shift_builtin*>()},
      {"shopt", boost::factory<shopt_builtin*>()},
      {"inherit", boost::factory<inherit_builtin*>()},
      {":", boost::factory<true_builtin*>()},
      {"true", boost::factory<true_builtin*>()},
      {"false", boost::factory<false_builtin*>()},
      {"return", boost::factory<return_builtin*>()},
      {"printf", boost::factory<printf_builtin*>()},
      {"let", boost::factory<let_builtin*>()},
      {"unset", boost::factory<unset_builtin*>()},
      {"read", boost::factory<read_builtin*>()},
      {"set", boost::factory<set_builtin*>()},
  });
  return *p;
}

void cppbash_builtin::transform_escapes(const std::string &string,
                                        std::ostream& output,
                                        bool ansi_c)
{
  using phoenix::val;
  using qi::lit;

  auto escape_parser =
  +(
    lit('\\') >>
    (
     lit('a')[output << val("\a")] |
     lit('b')[output << val("\b")] |
     // \e is a GNU extension
     lit('e')[output << val("\033")] |
     lit('E')[output << val("\033")] |
     lit('f')[output << val("\f")] |
     lit('n')[output << val("\n")] |
     lit('r')[output << val("\r")] |
     lit('t')[output << val("\t")] |
     lit('v')[output << val("\v")] |
     lit('\'')[output << val(ansi_c ? "'" : "\\'")] |
     lit('"')[output << val(ansi_c ? "\"" : "\\\"")] |
     lit('c')[phoenix::throw_(suppress_output())] |
     lit('\\')[output << val('\\')] |
     lit("0") >> qi::uint_parser<unsigned, 8, 1, 3>()[ output << phoenix::static_cast_<char>(qi::_1)] |
     lit("x") >> qi::uint_parser<unsigned, 16, 1, 2>()[ output << phoenix::static_cast_<char>(qi::_1)]

    ) |
    qi::char_[output << qi::_1]
  );

  auto begin = string.begin();
  qi::parse(begin, string.end(), escape_parser);
}
