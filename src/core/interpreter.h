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
/// \brief implementation for bash interpreter (visitor pattern)
///

#ifndef LIBBASH_CORE_INTERPRETER_H_
#define LIBBASH_CORE_INTERPRETER_H_

#include <functional>
#include <memory>
#include <string>

#include <boost/utility.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "core/function.h"
#include "core/symbols.hpp"
#include "cppbash_builtin.h"

/// \brief symbol table implementation
typedef std::unordered_map<std::string, std::shared_ptr<variable>> scope;

///
/// \class interpreter
/// \brief implementation for bash interpreter
///
class interpreter: public boost::noncopyable
{

  /// \brief global symbol table for variables
  scope members;

  /// \brief global symbol table for functions
  std::unordered_map<std::string, function> functions;

  std::stack<bash_ast*> ast_stack;

  /// \brief local scope for function arguments, execution environment and
  ///        local variables
  std::vector<scope> local_members;

  std::ostream* _out;

  std::ostream* _err;

  std::istream* _in;

  // std::map is chosen for sorted output in shopt -p
  std::map<std::string, bool> additional_options;

  // std::map is chosen for sorted output in $-. The order may not be the same
  // as bash implementation.
  std::map<char, bool> options;

  /// \brief the return status of the last command
  int status;

  /// \brief calculate the correct offset when offset < 0 and check whether
  ///        the real offset is in legal range
  /// \param[in,out] offset a value/result argument referring to offset
  /// \param[in] size the size of the original string
  /// \return whether the real offset is in legal range
  bool get_real_offset(long long& offset, const unsigned size) const
  {
    offset = (offset >= 0? offset : size + offset);
    return !(offset < 0 || offset >= size);
  }

  void get_all_elements_joined(const std::string& name,
                               const std::string& delim,
                               std::string& result) const;

  std::shared_ptr<variable> resolve_variable(const std::string&) const;

  void define_function_arguments(scope& current_stack,
                                 const std::vector<std::string>& arguments);

  std::string get_substring(const std::string& name,
                            long long offset,
                            unsigned length,
                            const unsigned index) const;

  std::string get_subarray(const std::string& name,
                           long long offset,
                           unsigned length) const;
public:

  /// bash option iterator
  typedef std::map<std::string, bool>::const_iterator option_iterator;

  ///
  /// \class local_scope
  /// \brief RAII concept for local scope management
  ///
  class local_scope
  {
    interpreter& walker;

  public:
    /// \brief construtor
    /// \param w the reference to the interpreter object
    local_scope(interpreter& w): walker(w)
    {
      walker.local_members.push_back(scope());
    }

    /// \brief the destructor
    ~local_scope()
    {
      walker.local_members.pop_back();
    }
  };

  /// \brief construtor
  interpreter();

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

  ///
  /// \brief checks whether the current scope is local or global
  /// \return whether current scope is local
  bool is_local_scope() const
  {
    return local_members.size() > 0;
  }

  /// \brief set current output stream
  /// \param stream the pointer to the output stream
  void set_output_stream(std::ostream* stream)
  {
    _out = stream;
  }

  /// \brief restore the current output stream to standard output stream
  void restore_output_stream()
  {
    _out = &std::cout;
  }

  /// \brief check whether a variable is valid and can be used
  /// \param var variable
  /// \return whether the variable is valid
  bool is_valid(const std::shared_ptr<variable>& var, const std::string& name) const
  {
    if(var)
      return true;

    if(get_option('u'))
      throw libbash::unsupported_exception(name + ": unbound variable");

    return false;
  }

  /// \brief resolve string/long variable, local scope will be
  ///        checked first, then global scope
  /// \param name variable name
  /// \param index array index, use index=0 if it's not an array
  /// \return the value of the variable, call default constructor if
  ///         it's undefined
  template <typename T>
  T resolve(const std::string& name, const unsigned index=0) const
  {
    auto var = resolve_variable(name);
    if(is_valid(var, name))
    {
      if(get_option('u') && var->is_unset(index))
      {
        if(name == "*")
          throw libbash::unsupported_exception("$" + boost::lexical_cast<std::string>(index) + ": unbound variable");
        else if(index == 0)
          throw libbash::unsupported_exception(name + ": unbound variable");
        else
          throw libbash::unsupported_exception(name + "[" + boost::lexical_cast<std::string>(index) + "]: unbound variable");
      }
      return var->get_value<T>(index);
    }
    else
    {
      if(get_option('u'))
        throw libbash::unsupported_exception(name + ": unbound variable");
      return T{};
    }
  }

  /// \brief resolve array variable
  /// \param name variable name
  /// \param[out] values vector that stores all array values
  template <typename T>
  bool resolve_array(const std::string& name, std::vector<T>& values) const
  {
    auto var = resolve_variable(name);
    if(!is_valid(var, name))
      return false;

    var->get_all_values(values);
    return true;
  }

  /// \brief check whether the value of the variable is null, return true
  ///        if the variable is undefined
  /// \param name variable name
  /// \param index array index, use index=0 if it's not an array
  /// \return whether the value of the variable is null
  bool is_unset_or_null(const std::string& name, const unsigned index) const;

  /// \brief check whether the value of the variable is unset
  /// \param name variable name
  /// \return whether the value of the variable is unset
  bool is_unset(const std::string& name) const
  {
    return members.find(name) == members.end();
  }

  /// \brief update the variable value, raise libbash::interpreter_exception if
  ///        it's readonly, will define the variable if it doesn't exist
  /// \param name variable name
  /// \param new_value new value
  /// \param index array index, use index=0 if it's not an array
  /// \return the new value of the variable
  template <typename T>
  const T& set_value(const std::string& name,
                     const T& new_value,
                     const unsigned index=0)
  {
    auto var = resolve_variable(name);
    if(var)
      var->set_value(new_value, index);
    else
      define(name, new_value, false, index);
    return new_value;
  }

  /// \brief set the return status of the last command
  /// \param s the value of the return status
  void set_status(int s)
  {
    status = s;
  }

  /// \brief get the return status of the last command
  int get_status(void) const
  {
    return status;
  }

  /// \brief unset a variable
  /// \param name the name of the variable
  void unset(const std::string& name);

  /// \brief unset a function
  /// \param name the name of the function
  void unset_function(const std::string& name);

  /// \brief unset a array member
  /// \param name the name of the array
  /// \param index the index of the member
  void unset(const std::string& name, const unsigned index);

  /// \brief define a new global variable
  /// \param name the name of the variable
  /// \param value the value of the variable
  /// \param readonly whether it's readonly, default is false
  /// \param index whether it's null, default is false
  template <typename T>
  void define(const std::string& name,
              const T& value,
              bool readonly=false,
              const unsigned index=0)
  {
    members[name].reset(new variable(name, value, readonly, index));
  }

  /// \brief define a new local variable
  /// \param name the name of the variable
  /// \param value the value of the variable
  /// \param readonly whether it's readonly, default is false
  /// \param index whether it's null, default is false
  template <typename T>
  void define_local(const std::string& name,
                    const T& value,
                    bool readonly=false,
                    const unsigned index=0)
  {
    if(local_members.empty())
      throw libbash::runtime_exception("Define local variables outside function scope");
    local_members.back()[name].reset(new variable(name, value, readonly, index));
  }

  /// \brief define a new function
  /// \param name the name of the function
  /// \param body_index the body index of the function
  void define_function(const std::string& name,
                       ANTLR3_MARKER body_index);

  /// \brief push current AST, used for function definition
  /// \param ast the pointer to the current ast
  void push_current_ast(bash_ast* ast)
  {
    ast_stack.push(ast);
  }

  /// \brief pop current AST, used for function definition
  void pop_current_ast()
  {
    ast_stack.pop();
  }

  /// \brief make function call
  /// \param name function name
  /// \param arguments function arguments
  void call(const std::string& name,
            const std::vector<std::string>& arguments);

  /// \brief check if we have 'name' defined as a function
  /// \param name function name
  /// \return whether 'name' is a function
  bool has_function(const std::string& name) const
  {
    return functions.find(name) != functions.end();
  }

  /// \brief get all defined function names
  /// \param[out] function_names the place to store the function names
  void get_all_function_names(std::vector<std::string>& function_names) const;

  /// \brief execute builtin
  /// \param name builtin name
  /// \param args builtin arguments
  /// \param output the output stream
  /// \param error the error stream
  /// \param input the input stream
  /// \return the return value of the builtin
  int execute_builtin(const std::string& name,
                      const std::vector<std::string>& args,
                      std::ostream* output=0,
                      std::ostream* error=0,
                      std::istream* input=0)
  {
    return cppbash_builtin::exec(name,
                                 args,
                                 output == 0 ? *_out : *output,
                                 error == 0 ? *_err : *error,
                                 input == 0 ? *_in : *input,
                                 *this);
  }

  /// \brief perform ${parameter:−word} expansion
  /// \param cond whether to perform expansion
  /// \param name the name of the parameter
  /// \param value the value of the word
  /// \param index the index of the paramter
  /// \return the expansion result
  const std::string do_default_expansion(bool cond,
                                         const std::string& name,
                                         const std::string& value,
                                         const unsigned index) const
  {
    return (cond ? value : resolve<std::string>(name, index));
  }

  /// \brief perform ${parameter:=word} expansion
  /// \param cond whether to perform expansion
  /// \param name the name of the parameter
  /// \param value the value of the word
  /// \param index the index of the paramter
  /// \return the expansion result
  const std::string do_assign_expansion(bool cond,
                                        const std::string& name,
                                        const std::string& value,
                                        const unsigned index)
  {
    return (cond ? set_value(name, value, index) : resolve<std::string>(name, index));
  }

  /// \brief perform ${parameter:+word} expansion
  /// \param cond whether to perform expansion
  /// \param value the value of the word
  /// \return the expansion result
  const std::string do_alternate_expansion(bool cond,
                                           const std::string& value) const
  {
    return (cond ? "" : value);
  }

  /// \brief perform substring expansion
  /// \param name the name of the parameter
  /// \param offset the offset of the substring
  /// \param index the index of the paramter
  /// \return the expansion result
  const std::string do_substring_expansion(const std::string& name,
                                           long long offset,
                                           const unsigned index) const;

  /// \brief perform substring expansion
  /// \param name the name of the parameter
  /// \param offset the offset of the substring
  /// \param length the length of the substring
  /// \param index the index of the paramter
  /// \return the expansion result
  const std::string do_substring_expansion(const std::string& name,
                                           long long offset,
                                           int length,
                                           const unsigned index) const;

  /// \brief perform subarray expansion
  /// \param name the name of the parameter
  /// \param offset the offset of the subarray
  /// \return the expansion result
  const std::string do_subarray_expansion(const std::string& name,
                                          long long offset) const;

  /// \brief perform subarray expansion
  /// \param name the name of the parameter
  /// \param offset the offset of the subarray
  /// \param length the length of the subarray
  /// \return the expansion result
  const std::string do_subarray_expansion(const std::string& name,
                                          long long offset,
                                          int length) const;

  /// \brief perform replacement expansion
  /// \param name the name of the varaible that needs to be expanded
  /// \param replacer the function object used to perform expansion
  /// \param index array index, use index=0 if it's not an array
  /// \return the expanded value
  std::string do_replace_expansion(const std::string& name,
                                   std::function<void(std::string&)> replacer,
                                   const unsigned index) const;

  /// \brief get the length of a string variable
  /// \param name the name of the variable
  /// \param index the index of the variable
  /// \return the length
  std::string::size_type get_length(const std::string& name, const unsigned index=0) const;

  /// \brief get the length of an array
  /// \param name the name of the array
  /// \return the length of the array
  variable::size_type get_array_length(const std::string& name) const;

  /// \brief get the max index of an array
  /// \param name the name of the array
  /// \return the max index of the array
  variable::size_type get_max_index(const std::string& name) const
  {
    auto var = resolve_variable(name);
    if(is_valid(var, name))
      return var->get_max_index();
    else
      return 0;
  }

  /// \brief get all array elements concatenated by space
  /// \param name the name of the array
  /// \param[out] result the concatenated string
  void get_all_elements(const std::string& name, std::string& result) const;

  /// \brief get all array elements concatenated by the first character of IFS
  /// \param name the name of the array
  /// \param[out] result the concatenated string
  void get_all_elements_IFS_joined(const std::string& name, std::string& result) const;

  /// \brief implementation of word splitting
  /// \param word the value of the word
  ///.\param[out] output the splitted result will be appended to output
  void split_word(const std::string& word, std::vector<std::string>& output) const;

  /// \brief get the status of shell optional behavior
  /// \param name the option name
  /// \return zero unless the name is not a valid shell option
  bool get_additional_option(const std::string& name) const;

  /// \brief set the status of shell optional behavior
  /// \param name the option name
  /// \param[in] value true if option is enabled, false otherwise
  /// \return zero unless the name is not a valid shell option
  void set_additional_option(const std::string& name, bool value);

  /// \brief get the status of shell optional behavior
  /// \param name the option name
  /// \return zero unless the name is not a valid shell option
  bool get_option(const char name) const;

  /// \brief set the status of shell optional behavior
  /// \param name the option name
  /// \param[in] value true if option is enabled, false otherwise
  /// \return zero unless the name is not a valid shell option
  void set_option(const char name, bool value);

  /// \brief return an iterator referring to the first variable
  /// \return iterator referring to the first variable
  option_iterator additional_options_begin() const
  {
    return additional_options.begin();
  }

  /// \brief return an iterator referring to the next element after the
  ///        last variable
  /// \return iterator referring to he next element after the last variable
  option_iterator additional_options_end() const
  {
    return additional_options.end();
  }

  /// \brief evaluate arithmetic expression and return the result
  /// \param expression the arithmetic expression
  /// \return the evaluated result
  long eval_arithmetic(const std::string& expression);

  /// \brief shift the positional parameters to the left by n.
  /// \param shift_number the number to be shifted
  /// \return zero unless n is greater than $# or less than zero, non-zero otherwise.
  int shift(int shift_number);

  void define_positional_arguments(const std::vector<std::string>::const_iterator begin,
                                   const std::vector<std::string>::const_iterator end);

  /// \brief perform expansion like ${var//foo/bar}
  /// \param value the value to be expanded
  /// \param pattern the pattern used to match the value
  /// \param replacement the replacement string
  static void replace_all(std::string& value,
                          const boost::xpressive::sregex& pattern,
                          const std::string& replacement);

  /// \brief perform expansion like ${var%foo}
  /// \param value the value to be expanded
  /// \param pattern the pattern used to match the value
  static void lazy_remove_at_end(std::string& value,
                                 const boost::xpressive::sregex& pattern);

  /// \brief perform expansion like ${var/foo/bar}
  /// \param value the value to be expanded
  /// \param pattern the pattern used to match the value
  /// \param replacement the replacement string
  static void replace_first(std::string& value,
                            const boost::xpressive::sregex& pattern,
                            const std::string& replacement);

  /// \brief remove trailing EOLs from the value
  /// \param[in, out] value the target
  static void trim_trailing_eols(std::string& value);
};
#endif
