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
/// \file interpreter.h
/// \author Mu Qiao
/// \brief implementation for bash interpreter (visitor pattern)
///

#ifndef LIBBASH_CORE_INTERPRETER_H_
#define LIBBASH_CORE_INTERPRETER_H_

#include <cmath>

#include <functional>
#include <memory>
#include <stack>
#include <string>

#include <antlr3basetree.h>

#include "core/symbols.hpp"
#include "cppbash_builtin.h"
#include "libbashLexer.h"

typedef std::unordered_map<std::string, std::shared_ptr<variable>> scope;
struct libbashWalker_Ctx_struct;
typedef struct libbashWalker_Ctx_struct * plibbashWalker;

///
/// \class interpreter
/// \brief implementation for bash interpreter
///
class interpreter{

  /// \var private::members
  /// \brief global symbol table for variables
  scope members;

  /// \var private::function_definitions
  /// \brief global symbol table for functions
  std::unordered_map<std::string, ANTLR3_MARKER> functions;

  /// \var private::local_members
  /// \brief local scope for function arguments, execution environment and
  ///        local variables
  std::stack<std::unique_ptr<scope>> local_members;

  std::ostream* out;

  std::ostream* err;

  std::istream* in;

  /// \brief calculate the correct offset when offset < 0 and check whether
  ///        the real offset is in legal range
  /// \param[in,out] a value/result argument referring to offset
  /// \param[in] the original string
  /// \return whether the real offset is in legal range
  bool get_real_offset(int& offset, const std::string& str) const
  {
    offset = (offset >= 0? offset : str.size() + offset);
    return !(offset < 0 || offset >= static_cast<int>(str.size()));
  }

  void get_all_elements_joined(const std::string& name,
                               const std::string& delim,
                               std::string& result) const;

public:

  interpreter(): out(&std::cout), err(&std::cerr), in(&std::cin)
  {
    define("IFS", " \t\n");
  }

  ///
  /// \brief return the number of variables
  /// \return the number of variables
  scope::size_type size() const
  {
    return members.size();
  }

  ///
  /// \brief return an iterator referring to the first variable
  /// \return iterator referring to the first variable
  scope::iterator begin()
  {
    return members.begin();
  }

  ///
  /// \brief return a const iterator referring to the first variable
  /// \return const iterator referring to the first variable
  scope::const_iterator begin() const
  {
    return members.begin();
  }

  ///
  /// \brief return an iterator referring to the next element after the
  ///        last variable
  /// \return iterator referring to he next element after the last variable
  scope::iterator end()
  {
    return members.end();
  }

  ///
  /// \brief return a const iterator referring to the next element after
  ///        the last variable
  /// \return const iterator referring to he next element after the last
  ///         variable
  scope::const_iterator end() const
  {
    return members.end();
  }

  void set_output_stream(std::ostream* stream)
  {
    out = stream;
  }

  void restore_output_stream()
  {
    out = &std::cout;
  }

  /// \brief parse the text value of a tree to integer
  /// \param the target tree
  /// \return the parsed value
  static int parse_int(ANTLR3_BASE_TREE* tree)
  {
    return tree->getText(tree)->toInt32(tree->getText(tree));
  }

  /// \brief a helper function that get the string value
  ///        of the given pANTLR3_BASE_TREE node.
  /// \param the target tree node
  /// \return the value of node->text
  static std::string get_string(pANTLR3_BASE_TREE node)
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

  /// \brief perform logic or
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int logicor(int left, int right)
  {
    return left || right;
  }

  /// \brief perform logic and
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int logicand(int left, int right)
  {
    return left && right;
  }

  /// \brief perform bitwise or
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int bitwiseor(int left, int right)
  {
    return left | right;
  }

  /// \brief perform bitwise and
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int bitwiseand(int left, int right)
  {
    return left & right;
  }

  /// \brief perform bitwise xor
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int bitwisexor(int left, int right)
  {
    return left ^ right;
  }

  /// \brief perform left or equal to
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int less_equal_than(int left, int right)
  {
    return left <= right;
  }

  /// \brief perform greater or equal to
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int greater_equal_than(int left, int right)
  {
    return left >= right;
  }

  /// \brief perform less than
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int less_than(int left, int right)
  {
    return left < right;
  }

  /// \brief perform greater than
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int greater_than(int left, int right)
  {
    return left > right;
  }

  /// \brief perform left shift
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int left_shift(int left, int right)
  {
    return left << right;
  }

  /// \brief perform right shift
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int right_shift(int left, int right)
  {
    return left >> right;
  }

  /// \brief perform plus
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int plus(int left, int right)
  {
    return left + right;
  }

  /// \brief perform minus
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int minus(int left, int right)
  {
    return left - right;
  }

  /// \brief perform multiply
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int multiply(int left, int right)
  {
    return left * right;
  }

  /// \brief perform divide
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int divide(int left, int right)
  {
    // We are not handling division by zero right now
    return left / right;
  }

  /// \brief perform modulo
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int mod(int left, int right)
  {
    return left % right;
  }

  /// \brief perform exponential operation
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int exp(int left, int right)
  {
    return pow(left, right);
  }

  /// \brief perform logic negation
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int negation(int element)
  {
    return !element;
  }

  /// \brief perform bitwise negation
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int bitwise_negation(int element)
  {
    return ~element;
  }


  /// \brief perform the ternary operator
  /// \param the condition
  /// \param the first operand
  /// \param the second operand
  /// \return the calculated result
  static int arithmetic_condition(int cnd, int left, int right)
  {
    return (cnd? left : right);
  }

  /// \brief perform pre-increment
  /// \param the variable name
  /// \return the increased value
  int pre_incr(const std::string& name, const unsigned index)
  {
    int value = resolve<int>(name, index);
    set_value(name, ++value);
    return value;
  }

  /// \brief perform pre-decrement
  /// \param the variable name
  /// \return the decreased value
  int pre_decr(const std::string& name, const unsigned index)
  {
    int value = resolve<int>(name, index);
    set_value(name, --value);
    return value;
  }

  /// \brief perform post-increment
  /// \param the variable name
  /// \return the original value
  int post_incr(const std::string& name, const unsigned index)
  {
    int value = resolve<int>(name, index);
    set_value(name, value + 1);
    return value;
  }

  /// \brief perform post-decrement
  /// \param the variable name
  /// \return the original value
  int post_decr(const std::string& name, const unsigned index)
  {
    int value = resolve<int>(name, index);
    set_value(name, value - 1);
    return value;
  }

  /// \brief assign with an operator (for example multiply)
  /// \param a function object to do an operation while assigning
  /// \param the name of the variable
  /// \param the value to assign
  /// \return the new value of the variable
  int assign(std::function<int(int,int)> f, const std::string& name, int value, const unsigned index)
  {
    int new_value = f(resolve<int>(name, index), value);
    set_value(name, new_value, index);
    return new_value;
  }

  /// \brief resolve string/int variable, local scope will be
  ///        checked first, then global scope
  /// \param variable name
  /// \param array index, use index=0 if it's not an array
  /// \return the value of the variable, call default constructor if
  ///         it's undefined
  template <typename T>
  T resolve(const std::string& name, const unsigned index=0) const
  {
    if(!local_members.empty())
    {
      auto iter_local = local_members.top()->find(name);
      if(iter_local != local_members.top()->end())
        return iter_local->second->get_value<T>(index);
    }

    auto iter_global = members.find(name);
    if(iter_global == members.end())
      return T();
    return iter_global->second->get_value<T>(index);
  }

  /// \brief resolve array variable
  /// \param variable name
  /// \param[out] vector that stores all array values
  template <typename T>
  void resolve_array(const std::string& name, std::vector<T>& values) const
  {
    auto i = members.find(name);
    if(i == members.end())
      return;

    i->second->get_all_values(values);
  }

  /// \brief check whether the value of the variable is null, return true
  ///        if the variable is undefined
  /// \param variable name
  /// \return whether the value of the variable is null
  bool is_unset_or_null(const std::string& name, const unsigned index) const
  {
    auto i = members.find(name);
    if(i == members.end())
      return true;
    else
      return i->second->is_null(index);
  }

  /// \brief check whether the value of the variable is unset
  /// \param variable name
  /// \return whether the value of the variable is unset
  bool is_unset(const std::string& name) const
  {
    return members.find(name) == members.end();
  }

  /// \brief update the variable value, raise interpreter_exception if
  ///        it's readonly, will define the variable if it doesn't exist
  /// \param variable name
  /// \param new value
  /// \param array index, use index=0 if it's not an array
  /// \return the new value of the variable
  template <typename T>
  const T& set_value(const std::string& name,
                     const T& new_value,
                     const unsigned index=0,
                     bool is_null=false)
  {
    auto i = members.find(name);
    if(i == members.end())
      define(name, new_value, false, is_null, index);
    else
      i->second->set_value(new_value, index, is_null);
    return new_value;
  }

  /// \brief define a new global variable
  /// \param the name of the variable
  /// \param the value of the variable
  /// \param whether it's readonly, default is false
  /// \param whether it's null, default is false
  template <typename T>
  void define(const std::string& name,
              const T& value,
              bool readonly=false,
              bool is_null=false,
              const unsigned index=0)
  {
    std::shared_ptr<variable> target(
        new variable(name, value, readonly, is_null, index));
    members[name] = target;
  }

  /// \brief define a new function
  /// \param the name of the function
  /// \param the body index of the function
  void define_function(const std::string& name,
                       ANTLR3_MARKER body_index)
  {
    functions[name] = body_index;
  }

  /// \brief make function call
  /// \param function name
  /// \param function arguments
  /// \param walker context
  /// \param the function that needs to be executed
  /// \return the return value of the function
  int call(const std::string& name,
           const std::vector<std::string>& arguments,
           plibbashWalker ctx,
           std::function<void(plibbashWalker)> f);

  /// \brief check if we have 'name' defined as a function
  /// \param function name
  /// \return whether 'name' is a function
  bool has_function(const std::string& name)
  {
    return functions.find(name) != functions.end();
  }

  void get_all_function_names(std::vector<std::string>& function_names);

  /// \brief execute builtin
  /// \param builtin name
  /// \param builtin arguments
  /// \return the return value of the builtin
  int execute_builtin(const std::string& name,
                      const std::vector<std::string>& args)
  {
    return cppbash_builtin::exec(name, args, *out, *err, *in);
  }

  /// \brief perform ${parameter:−word} expansion
  /// \param the name of the parameter
  /// \param the value of the word
  /// \return the expansion result
  const std::string do_default_expansion(const std::string& name,
                                         const std::string& value,
                                         const unsigned index) const
  {
    return (is_unset_or_null(name, index)?
        value : resolve<std::string>(name, index));
  }

  /// \brief perform ${parameter:=word} expansion
  /// \param the name of the parameter
  /// \param the value of the word
  /// \return the expansion result
  const std::string do_assign_expansion(const std::string& name,
                                        const std::string& value,
                                        const unsigned index)
  {
    return (is_unset_or_null(name, index)?
        set_value(name, value, index) : resolve<std::string>(name, index));
  }

  /// \brief perform ${parameter:+word} expansion
  /// \param the name of the parameter
  /// \param the value of the word
  /// \return the expansion result
  const std::string do_alternate_expansion(const std::string& name,
                                           const std::string& value,
                                           const unsigned index) const
  {
    return (is_unset_or_null(name, index)? "" : value);
  }

  /// \brief perform substring expansion
  /// \param the offset of the substring
  /// \return the expansion result
  const std::string do_substring_expansion(const std::string& name,
                                           int offset,
                                           const unsigned index) const
  {
    std::string value = resolve<std::string>(name, index);
    if(!get_real_offset(offset, value))
      return "";
    return value.substr(offset);
  }

  /// \brief perform substring expansion
  /// \param the offset of the substring
  /// \param the length of the substring
  /// \return the expansion result
  const std::string do_substring_expansion(const std::string& name,
                                           int offset,
                                           int length,
                                           const unsigned index) const
  {
    if(length < 0)
      throw interpreter_exception("length of substring expression should be greater or equal to zero");
    std::string value = resolve<std::string>(name, index);
    if(!get_real_offset(offset, value))
      return "";
    return value.substr(offset, length);
  }

  /// \brief perform replacement expansion
  /// \param the name of the varaible that needs to be expanded
  /// \param the function object used to perform expansion
  /// \param array index, use index=0 if it's not an array
  /// \return the expanded value
  std::string do_replace_expansion(const std::string& name,
                                   std::function<void(std::string&)> replacer,
                                   const unsigned index) const
  {
    std::string value = resolve<std::string>(name, index);
    replacer(value);
    return value;
  }

  /// \brief get the length of a string variable
  /// \param the name of the variable
  /// \return the length
  unsigned get_length(const std::string& name, const unsigned index=0) const
  {
    auto i = members.find(name);
    if(i == members.end())
      return 0;
    return i->second->get_length(index);
  }

  /// \brief get the length of an array
  /// \param the name of the array
  /// \return the length of the array
  unsigned get_array_length(const std::string& name) const
  {
    auto i = members.find(name);
    if(i == members.end())
      return 0;
    else
      return i->second->get_array_length();
  }

  /// \brief get all array elements concatenated by space
  /// \param the name of the array
  /// \param[out] the concatenated string
  void get_all_elements(const std::string&, std::string&) const;

  /// \brief get all array elements concatenated by the first character of IFS
  /// \param the name of the array
  /// \param[out] the concatenated string
  void get_all_elements_IFS_joined(const std::string&, std::string&) const;

  /// \brief implementation of word splitting
  /// \param the value of the word
  //. \param[out] the splitted result
  void split_word(const std::string& word, std::vector<std::string>& output);

  /// \brief perform expansion like ${var//foo/bar}
  /// \param the value to be expanded
  /// \param the pattern used to match the value
  /// \param the replacement string
  static void replace_all(std::string& value,
                          const std::string& pattern,
                          const std::string& replacement);

  /// \brief perform expansion like ${var/%foo/bar}
  /// \param the value to be expanded
  /// \param the pattern used to match the value
  /// \param the replacement string
  static void replace_at_end(std::string& value,
                             const std::string& pattern,
                             const std::string& replacement);

  /// \brief perform expansion like ${var/#foo/bar}
  /// \param the value to be expanded
  /// \param the pattern used to match the value
  /// \param the replacement string
  static void replace_at_start(std::string& value,
                               const std::string& pattern,
                               const std::string& replacement);

  /// \brief perform expansion like ${var/foo/bar}
  /// \param the value to be expanded
  /// \param the pattern used to match the value
  /// \param the replacement string
  static void replace_first(std::string& value,
                            const std::string& pattern,
                            const std::string& replacement);

  /// \brief perform expansion like ${var#foo}
  /// \param the value to be expanded
  /// \param the pattern used to match the value
  static void lazy_remove_at_start(std::string& value,
                                   const std::string& pattern);

  /// \brief perform expansion like ${var%foo}
  /// \param the value to be expanded
  /// \param the pattern used to match the value
  static void lazy_remove_at_end(std::string& value,
                                 const std::string& pattern);

  /// \brief remove trailing EOLs from the value
  /// \param[in, out] the target
  static void trim_trailing_eols(std::string& value);
};
#endif
