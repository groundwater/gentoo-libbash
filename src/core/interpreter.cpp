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
/// \file interpreter.cpp
/// \author Mu Qiao
/// \brief implementations for bash interpreter (visitor pattern).
///

#include "core/interpreter.h"

#include <functional>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "libbashWalker.h"

void interpreter::get_all_elements_joined(const std::string& name,
                                          const std::string& delim,
                                          std::string& result) const
{
  std::vector<std::string> source;

  auto i = members.find(name);
  if(i != members.end())
  {
    i->second->get_all_values(source);
    result = boost::algorithm::join(source, delim);
  }
  else
  {
    result = "";
  }
}

void interpreter::get_all_elements(const std::string& name,
                                   std::string& result) const
{
  get_all_elements_joined(name, " ", result);
}

void interpreter::get_all_elements_IFS_joined(const std::string& name,
                                              std::string& result) const
{
  get_all_elements_joined(name,
                          resolve<std::string>("IFS").substr(0, 1),
                          result);
}

void interpreter::split_word(const std::string& word, std::vector<std::string>& output)
{
  const std::string& delimeter = resolve<std::string>("IFS");
  boost::split(output, word, boost::is_any_of(delimeter), boost::token_compress_on);
}

inline void define_function_arguments(std::unique_ptr<scope>& current_stack,
                                      const std::vector<std::string>& arguments)
{
  for(auto i = 0u; i != arguments.size(); ++i)
  {
    const std::string& name = boost::lexical_cast<std::string>(i + 1);
    (*current_stack)[name] = std::shared_ptr<variable>(new variable(name, arguments[i]));
  }
}

int interpreter::call(const std::string& name,
                       const std::vector<std::string>& arguments,
                       plibbashWalker ctx,
                       std::function<void(plibbashWalker)> f)
{
  ANTLR3_MARKER func_index;
  auto iter = functions.find(name);
  if(iter == functions.end())
    return -1;
  func_index = iter->second;

  // Prepare function stack and arguments
  local_members.push(std::unique_ptr<scope>(new scope));
  define_function_arguments(local_members.top(), arguments);

  auto INPUT = ctx->pTreeParser->ctnstream;
  auto ISTREAM = INPUT->tnstream->istream;
  // Saving current index
  ANTLR3_MARKER curr = ISTREAM->index(ISTREAM);
  // Push function index into INPUT
  INPUT->push(INPUT, func_index);
  // Execute function body
  f(ctx);
  // Reset to the previous index
  ISTREAM->seek(ISTREAM, curr);

  // Clear function stack
  local_members.pop();

  return 0;
}

void interpreter::replace_all(std::string& value,
                              const std::string& pattern,
                              const std::string& replacement)
{
  boost::replace_all(value, pattern, replacement);
}

void interpreter::replace_at_end(std::string& value,
                                 const std::string& pattern,
                                 const std::string& replacement)
{
  if(value.size() >= pattern.size() &&
     value.substr(value.size() - pattern.size()) == pattern)
    value.replace(value.size() - pattern.size(),
                  pattern.size(),
                  replacement);
}

void interpreter::replace_at_start(std::string& value,
                                   const std::string& pattern,
                                   const std::string& replacement)
{
  if(value.substr(0, pattern.size()) == pattern)
    value.replace(0, pattern.size(), replacement);
}

void interpreter::replace_first(std::string& value,
                                const std::string& pattern,
                                const std::string& replacement)
{
  boost::replace_first(value, pattern, replacement);
}

void interpreter::lazy_remove_at_start(std::string& value,
                                       const std::string& pattern)
{
  replace_at_start(value, pattern, "");
}

void interpreter::lazy_remove_at_end(std::string& value,
                                     const std::string& pattern)
{
  replace_at_end(value, pattern, "");
}

void interpreter::trim_trailing_eols(std::string& value)
{
  boost::trim_right_if(value, boost::is_any_of("\n"));
}

void interpreter::get_all_function_names(std::vector<std::string>& function_names)
{
  boost::copy(functions | boost::adaptors::map_keys, back_inserter(function_names));
}
