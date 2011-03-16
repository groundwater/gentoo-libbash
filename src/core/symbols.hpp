/*
   Copyright 2011 Mu Qiao

   This file is part of libbash.

   libbash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   libbash is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of
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
  symbol(){}

  symbol(const std::string& n): name(n){}

  /// \brief retrieve symbol name
  /// \return const string value of symbol name
  const std::string& get_name() const
  {
    return name;
  }

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

public:
  template <typename T>
  variable(const std::string& name, T v, bool ro=false)
    : symbol(name), value(v), readonly(ro){}

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
  template <typename T>
  void set_value(T new_value)
  {
    if(readonly)
      throw interpreter_exception(get_name() + " is readonly variable");
    value = new_value;
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
