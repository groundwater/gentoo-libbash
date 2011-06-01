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

#include <cctype>

#include <functional>

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

#include "core/interpreter.h"

interpreter::interpreter(): out(&std::cout), err(&std::cerr), in(&std::cin), bash_options(
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
  return std::string(reinterpret_cast<const char *>(token->start),
                     token->stop - token->start + 1);
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
                                       int offset,
                                       int length,
                                       const unsigned index) const
{
  if(name != "*" && name != "@")
  {
    std::string value = resolve<std::string>(name, index);
    if(!get_real_offset(offset, value.size()))
      return "";
    return value.substr(offset, length);
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
    if(resolve_array(name, array) && get_real_offset(offset, array.size()))
    {
      int max_length = array.size() - offset;
      if(length == -1 || length > max_length)
        length = max_length;
      return boost::algorithm::join(
          std::vector<std::string>(array.begin() + offset,
                                   array.begin() + offset + length),
          resolve<std::string>("IFS").substr(0, 1));
    }
    else
      return "";
  }
}

const std::string interpreter::do_substring_expansion(const std::string& name,
                                                      int offset,
                                                      const unsigned index) const
{
  return get_substring(name, offset, -1, index);
}

const std::string interpreter::do_substring_expansion(const std::string& name,
                                                      int offset,
                                                      int length,
                                                      const unsigned index) const
{
  if(length < 0)
    throw interpreter_exception("length of substring expression should be greater or equal to zero");

  return get_substring(name, offset, length, index);
}

std::string interpreter::do_replace_expansion(const std::string& name,
                                              std::function<void(std::string&)> replacer,
                                              const unsigned index) const
{
  std::string value = resolve<std::string>(name, index);
  replacer(value);
  return value;
}

unsigned interpreter::get_length(const std::string& name,
                                 const unsigned index) const
{
  auto i = members.find(name);
  if(i == members.end())
    return 0;
  return i->second->get_length(index);
}

unsigned interpreter::get_array_length(const std::string& name) const
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
  std::map<int, std::string> positional_args;

  for(auto i = 0u; i != arguments.size(); ++i)
  {
    const std::string& name = boost::lexical_cast<std::string>(i + 1);
    current_stack[name].reset(new variable(name, arguments[i]));
    positional_args[i] = arguments[i];
  }

  define("*", positional_args);
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

  // Prepare arguments
  define_function_arguments(local_members.back(), arguments);

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

  return 0;
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
    throw interpreter_exception("Invalid bash option");

  iter->second = value;
}

int interpreter::eval_arithmetic(const std::string& expression)
{
  bash_ast ast(std::stringstream(expression), &bash_ast::parser_arithmetics);
  return ast.interpret_with(*this, &bash_ast::walker_arithmetics);
}
