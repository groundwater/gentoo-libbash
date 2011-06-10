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
/// \brief implementations for bash interpreter (visitor pattern).
///
#include "core/interpreter.h"

#include <cctype>

#include <functional>
#include <limits>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "core/bash_ast.h"
#include "core/unset_exception.h"
#include "libbashWalker.h"

interpreter::interpreter(): _out(&std::cout), _err(&std::cerr), _in(&std::cin), bash_options(
    {
      {"autocd", false},
      {"cdable_vars", false},
      {"cdspell", false},
      {"checkhash", false},
      {"checkjobs", false},
      {"checkwinsize", false},
      {"cmdhist", false},
      {"compat31", false},
      {"dirspell", false},
      {"dotglob", false},
      {"execfail", false},
      {"expand_aliases", false},
      {"extdebug", false},
      {"extglob", false},
      {"extquote", false},
      {"failglob", false},
      {"force_fignore", false},
      {"globstar", false},
      {"gnu_errfmt", false},
      {"histappend", false},
      {"histreedit", false},
      {"histverify", false},
      {"hostcomplete", false},
      {"huponexit", false},
      {"interactive", false},
      {"lithist", false},
      {"login_shell", false},
      {"mailwarn", false},
      {"no_empty_cmd_completion", false},
      {"nocaseglob", false},
      {"nocasematch", false},
      {"nullglob", false},
      {"progcomp", false},
      {"promptvars", false},
      {"restricted", false},
      {"shift_verbose", false},
      {"sourcepath", false},
      {"xpg_echo", false},
    }
    )
{
  define("IFS", " \t\n");
}

std::string interpreter::get_string(pANTLR3_BASE_TREE node)
{
  pANTLR3_COMMON_TOKEN token = node->getToken(node);
  // The tree walker may send null pointer here, so return an empty
  // string if that's the case.
  if(!token->start)
    return "";
  // Use reinterpret_cast here because we have to cast C code.
  // The real type here is int64_t which is used as a pointer.
  // token->stop - token->start + 1 should be bigger than 0.
  return std::string(reinterpret_cast<const char *>(token->start),
                     boost::numeric_cast<unsigned>(token->stop - token->start + 1));
}

std::shared_ptr<variable> interpreter::resolve_variable(const std::string& name) const
{
  if(name.empty())
    return std::shared_ptr<variable>();

  // positional parameter
  if(isdigit(name[0]) && !local_members.empty())
  {
    auto iter_local = local_members.back().find(name);
    if(iter_local != local_members.back().end())
      return iter_local->second;
  }
  else
  {
    BOOST_REVERSE_FOREACH(auto& frame, local_members)
    {
      auto iter_local = frame.find(name);
      if(iter_local != frame.end())
        return iter_local->second;
    }
  }

  auto iter_global = members.find(name);
  if(iter_global == members.end())
    return std::shared_ptr<variable>();
  return iter_global->second;
}

bool interpreter::is_unset_or_null(const std::string& name,
                                   const unsigned index) const
{
  auto i = members.find(name);
  if(i == members.end())
    return true;
  else
    return i->second->is_null(index);
}

// This method temporarily supports array offset expansion for $* and $@.
// That logic will be refactored and applied to normal array variables in future.
std::string interpreter::get_substring(const std::string& name,
                                       long long offset,
                                       unsigned length,
                                       const unsigned index) const
{
  if(name != "*" && name != "@")
  {
    std::string value = resolve<std::string>(name, index);
    if(!get_real_offset(offset, boost::numeric_cast<unsigned>(value.size())))
      return "";
    // After get_real_offset, we know offset can be cast to unsigned.
    return value.substr(boost::numeric_cast<std::string::size_type>(offset), length);
  }
  else
  {
    std::vector<std::string> array;
    // ${*:1} has the same content as ${*}, ${*:0} contains current script name as the first element
    if(offset > 0)
      offset--;
    else if(offset == 0)
      // Need to replace this with the real script name
      array.push_back("filename");
    // We do not support arrays that have size bigger than numeric_limits<unsigned>::max()
    if(resolve_array(name, array) && get_real_offset(offset, boost::numeric_cast<unsigned>(array.size())))
    {
      // We do not support arrays that have size bigger than numeric_limits<unsigned>::max()
      // After get_real_offset, we know offset can be cast to unsigned.
      unsigned max_length = boost::numeric_cast<unsigned>(array.size()) - boost::numeric_cast<unsigned>(offset);
      if(length > max_length)
        length = max_length;

      auto start = array.begin() + boost::numeric_cast<std::vector<std::string>::difference_type>(offset);
      auto end = array.begin() + boost::numeric_cast<std::vector<std::string>::difference_type>(offset + length);
      return boost::algorithm::join(std::vector<std::string>(start, end), resolve<std::string>("IFS").substr(0, 1));
    }
    else
      return "";
  }
}

const std::string interpreter::do_substring_expansion(const std::string& name,
                                                      long long offset,
                                                      const unsigned index) const
{
  return get_substring(name, offset, std::numeric_limits<unsigned>::max(), index);
}

const std::string interpreter::do_substring_expansion(const std::string& name,
                                                      long long offset,
                                                      int length,
                                                      const unsigned index) const
{
  if(length < 0)
    throw interpreter_exception("length of substring expression should be greater or equal to zero");

  return get_substring(name, offset, boost::numeric_cast<unsigned>(length), index);
}

std::string interpreter::do_replace_expansion(const std::string& name,
                                              std::function<void(std::string&)> replacer,
                                              const unsigned index) const
{
  std::string value = resolve<std::string>(name, index);
  replacer(value);
  return value;
}

std::string::size_type interpreter::get_length(const std::string& name,
                                 const unsigned index) const
{
  auto i = members.find(name);
  if(i == members.end())
    return 0;
  return i->second->get_length(index);
}

variable::size_type interpreter::get_array_length(const std::string& name) const
{
  auto i = members.find(name);
  if(i == members.end())
    return 0;
  else
    return i->second->get_array_length();
}

void interpreter::get_all_elements_joined(const std::string& name,
                                          const std::string& delim,
                                          std::string& result) const
{
  std::vector<std::string> array;

  if(resolve_array(name, array))
    result = boost::algorithm::join(array, delim);
  else
    result = "";
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
  std::string trimmed(word);
  boost::trim_if(trimmed, boost::is_any_of(delimeter));

  if(trimmed == "")
    return;

  std::vector<std::string> splitted_values;
  boost::split(splitted_values, trimmed, boost::is_any_of(delimeter), boost::token_compress_on);
  output.insert(output.end(), splitted_values.begin(), splitted_values.end());
}

void interpreter::define_function_arguments(scope& current_stack,
                                            const std::vector<std::string>& arguments)
{
  std::map<unsigned, std::string> positional_args;

  for(auto i = 0u; i != arguments.size(); ++i)
  {
    const std::string& name = boost::lexical_cast<std::string>(i + 1);
    current_stack[name].reset(new variable(name, arguments[i]));
    positional_args[i] = arguments[i];
  }

  define("*", positional_args);
}

void interpreter::call(const std::string& name,
                       const std::vector<std::string>& arguments)
{
  // Prepare arguments
  define_function_arguments(local_members.back(), arguments);

  auto iter = functions.find(name);
  if(iter != functions.end())
    iter->second.call(*this);
  else
    throw interpreter_exception(name + " is not defined.");
}

void interpreter::replace_all(std::string& value,
                              const boost::xpressive::sregex& pattern,
                              const std::string& replacement)
{
  value = boost::xpressive::regex_replace(value,
                                          pattern,
                                          replacement,
                                          boost::xpressive::regex_constants::format_literal);
}

void interpreter::lazy_remove_at_end(std::string& value,
                              const boost::xpressive::sregex& pattern)
{
  boost::xpressive::smatch what;
  if(boost::xpressive::regex_match(value,
                                   what,
                                   pattern))
    value = what[1];
}

void interpreter::replace_first(std::string& value,
                                const boost::xpressive::sregex& pattern,
                                const std::string& replacement)
{
  value = boost::xpressive::regex_replace(value,
                                          pattern,
                                          replacement,
                                          boost::xpressive::regex_constants::format_literal | boost::xpressive::regex_constants::format_first_only);
}

void interpreter::trim_trailing_eols(std::string& value)
{
  boost::trim_right_if(value, boost::is_any_of("\n"));
}

void interpreter::get_all_function_names(std::vector<std::string>& function_names)
{
  boost::copy(functions | boost::adaptors::map_keys, back_inserter(function_names));
}

namespace
{
  void check_unset_positional(const std::string& name)
  {
    // Unsetting positional parameters is not allowed
    if(isdigit(name[0]))
      throw unset_exception("unset: not a valid identifier");
  }
}

void interpreter::unset(const std::string& name)
{
  check_unset_positional(name);

  auto unsetter = [&](scope& frame) -> bool {
    auto iter_local = frame.find(name);
    if(iter_local != frame.end())
    {
      if(iter_local->second->is_readonly())
        throw unset_exception("unset a readonly variable");
      frame.erase(iter_local);
      return true;
    }
    return false;
  };

  if(std::none_of(local_members.rbegin(), local_members.rend(), unsetter))
    unsetter(members);
}

// We need to return false when unsetting readonly functions in future
void interpreter::unset_function(const std::string& name)
{
  auto function = functions.find(name);
  if(function != functions.end())
    functions.erase(name);
}

void interpreter::unset(const std::string& name,
                        const unsigned index)
{
  check_unset_positional(name);

  auto var = resolve_variable(name);
  if(var)
  {
    if(var->is_readonly())
      throw unset_exception("unset a readonly variable");
    var->unset_value(index);
  }
}

bool interpreter::get_option(const std::string& name) const
{
  auto iter = bash_options.find(name);
  if(iter == bash_options.end())
    throw interpreter_exception("Invalid bash option");

  return iter->second;
}

void interpreter::set_option(const std::string& name, bool value)
{
  auto iter = bash_options.find(name);
  if(iter == bash_options.end())
    throw interpreter_exception(name + " is not a valid bash option");

  iter->second = value;
}

int interpreter::eval_arithmetic(const std::string& expression)
{
  bash_ast ast(std::stringstream(expression), &bash_ast::parser_arithmetics);
  return ast.interpret_with(*this, &bash_ast::walker_arithmetics);
}
