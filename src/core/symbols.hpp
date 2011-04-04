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
/// \file symbols.hpp
/// \author Mu Qiao
/// \brief template implementation for symbols and symbol table
///

#ifndef SYMBOLS_HPP_
#define SYMBOLS_HPP_

#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>

#include "core/interpreter_exception.h"

///
/// \class symbol
/// \brief base class for symbols such as variables and functions
///
class symbol
{
  /// \var private::name
  /// \brief symbol name
  std::string name;

public:
  /// \brief retrieve symbol name
  /// \return const string value of symbol name
  const std::string& get_name() const
  {
    return name;
  }

protected:
  symbol(const std::string& n): name(n){}
  ~symbol() {}
};

///
/// \class converter
/// \brief template class of converter
///
template<typename T>
class converter: public boost::static_visitor<T>
{};

///
/// \class converter
/// \brief specialized converter for int
///
template<>
class converter<int>: public boost::static_visitor<int>
{
public:
  /// \brief converter for int value
  /// \param the value to be converted
  /// \return the converted int
  int operator() (const int value) const
  {
    return value;
  }

  /// \brief converter for string value
  /// \param the value to be converted
  /// \return the converted int
  int operator() (const std::string& value) const
  {
    int result = 0;
    try
    {
      result = boost::lexical_cast<int>(value);
    }
    catch(boost::bad_lexical_cast& e)
    {
      throw interpreter_exception("can't cast " + value + " to int");
    }
    return result;
  }
};

///
/// \class converter
/// \brief specialized converter for string
///
template<>
class converter<std::string>:
  public boost::static_visitor<std::string>
{
public:
  /// \brief converter for int value
  /// \param the value to be converted
  /// \return the converted string
  std::string operator() (const int value) const
  {
    return boost::lexical_cast<std::string>(value);
  }

  /// \brief converter for string value
  /// \param the value to be converted
  /// \return the converted string
  std::string operator() (const std::string& value) const
  {
    return value;
  }
};

///
/// \class variable
/// \brief implementation for all variable types
///
class variable: public symbol
{
  /// \var private::value
  /// \brief actual value of the symbol
  boost::variant<int, std::string> value;

  /// \var private::readonly
  /// \brief whether the symbol is readonly
  bool readonly;

  /// \var private::null_value
  /// \brief whether the symbol is null 
  bool null_value;

public:
  template <typename T>
  variable(const std::string& name,
           T v,
           bool ro=false,
           bool is_null=false)
    : symbol(name), value(v), readonly(ro), null_value(is_null){}

  /// \brief retrieve actual value of the symbol
  /// \return the value of the symbol
  template<typename T>
  T get_value() const
  {
    static converter<T> visitor;
    return boost::apply_visitor(visitor, value);
  }

  /// \brief set the value of the symbol, raise exception if it's readonly
  /// \param the new value to be set
  /// \param whether to set the variable to null value, default is false
  template <typename T>
  void set_value(T new_value, bool is_null=false)
  {
    if(readonly)
      throw interpreter_exception(get_name() + " is readonly variable");
    null_value = is_null;
    value = new_value;
  }

  /// \brief check whether the value of the variable is null
  /// \return whether the value of the variable is null
  bool is_null() const
  {
    return null_value;
  }
};

///
/// class scope
/// \brief implementation for symbol table
///
class scope
{
public:
  typedef std::unordered_map<std::string, std::shared_ptr<symbol>>
    table_type;
  typedef table_type::iterator iterator;
  typedef table_type::const_iterator const_iterator;
  typedef table_type::size_type size_type;
  typedef table_type::value_type value_type;

  ///
  /// \brief return the number of symbols in current scope
  /// \return the number of symbols
  size_type size()
  {
    return members.size();
  }

  ///
  /// \brief return an iterator referring to the first symbol
  /// \return iterator referring to the first symbol
  iterator begin()
  {
    return members.begin();
  }

  ///
  /// \brief return a const iterator referring to the first symbol
  /// \return const iterator referring to the first symbol
  const_iterator begin() const
  {
    return members.begin();
  }

  ///
  /// \brief return an iterator referring to the next element after
  ///        the last symbol in current scope
  /// \return iterator referring to he next element after the last
  ///         symbol in current scope
  iterator end()
  {
    return members.end();
  }

  ///
  /// \brief return a const iterator referring to the next element
  ///        after the last symbol in current scope
  /// \return const iterator referring to he next element after the
  ///         last symbol in current scope
  const_iterator end() const
  {
    return members.end();
  }

  /// \brief define a new symbol
  /// \param the new symbol
  void define(std::shared_ptr<symbol> s)
  {
    members[s->get_name()] = s;
  }

  /// \brief resolve a symbol
  /// \param the symbol name
  /// \return target symbol passed by reference
  std::shared_ptr<symbol> resolve(const std::string& name)
  {
    return members[name];
  }
protected:
  /// \var protected::member
  /// \brief symbol table data structure
  table_type members;
};
#endif
