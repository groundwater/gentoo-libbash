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
/// \file read_builtin.cpp
/// \brief class that implements the read builtin
///

#include "builtins/read_builtin.h"

#include <string.h>
#include <boost/algorithm/string.hpp>

#include "core/interpreter.h"
#include "builtins/builtin_exceptions.h"

void read_builtin::process(const std::vector<std::string>& args, const std::string& input)
{
  std::vector<std::string> split_input;
  boost::split(split_input, input, boost::is_any_of(" "));

  auto vars = args.begin();
  for(auto words = split_input.begin(); vars != args.end() && words != split_input.end(); ++vars, ++words)
  {
    if(vars != args.end() - 1)
    {
      _walker.set_value(*vars, *words);
    }
    else
    {
      std::string rest;
      for(; words != split_input.end() - 1; ++words)
        rest += *words + " ";
      rest += *words;
      _walker.set_value(*vars, rest);
    }
  }

  for(; vars != args.end(); ++vars)
    _walker.set_value(*vars, "");
}

int read_builtin::exec(const std::vector<std::string>& bash_args)
{
  int return_value = 0;
  std::string input;
  std::stringstream formated_input;

  getline(this->input_buffer(), input);

  if(this->input_buffer().eof())
    return_value = 1;

  if(input.size() < 1)
    return return_value;

  while(input[input.length()-1] == '\\') {
    input.erase(input.end()-1);
    std::string input_line;
    getline(this->input_buffer(), input_line);

    if(this->input_buffer().eof())
      return_value = 1;

    if(input.size() < 1)
      return return_value;

    input += input_line;
  }

  cppbash_builtin::transform_escapes(input, formated_input, false);

  if(bash_args.empty())
    process({"REPLY"}, formated_input.str());
  else
    process(bash_args, formated_input.str());

  return return_value;
}
