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

#ifndef LIBBASH_CORE_SYMBOLS_HPP_
#define LIBBASH_CORE_SYMBOLS_HPP_

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>

#include "core/interpreter_exception.h"

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
class variable
{
  /// \var private::name
  /// \brief variable name
  std::string name;

  /// \var private::value
  /// \brief actual value of the variable. We put string in front of int
  ///        because we want "" as default string value; Otherwise we
  ///        will get "0".
  std::map<int, boost::variant<std::string, int>> value;

  /// \var private::readonly
  /// \brief whether the variable is readonly
  bool readonly;

public:
  /// \brief retrieve variable name
  /// \return const string value of variable name
  const std::string& get_name() const
  {
    return name;
  }

  template <typename T>
  variable(const std::string& name,
           const T& v,
           bool ro=false,
           bool is_null=false,
           const unsigned index=0)
    : name(name), readonly(ro)
  {
    if(!is_null)
        value[index] = v;
  }

  /// \brief retrieve actual value of the variable, if index is out of bound,
  ///        will return the default value of type T
  /// \return the value of the variable
  template<typename T>
  T get_value(const unsigned index=0) const
  {
    static converter<T> visitor;

    auto iter = value.find(index);
    if(iter == value.end())
        return T{};

    return boost::apply_visitor(visitor, iter->second);
  }

  /// \brief retrieve all values of the array
  /// \param[out] vector that stores all array values, values in the arrays will
  ///             be cleared first
  template<typename T>
  void get_all_values(std::vector<T>& all_values) const
  {
    static converter<T> visitor;

    all_values.clear();

    for(auto iter = value.begin(); iter != value.end(); ++iter)
        all_values.push_back(
                boost::apply_visitor(visitor, iter->second));
  }


  /// \brief set the value of the variable, raise exception if it's readonly
  /// \param the new value to be set
  /// \param array index, use index=0 if it's not an array
  /// \param whether to set the variable to null value, default is false
  template <typename T>
  void set_value(const T& new_value,
                 const unsigned index=0,
                 bool is_null=false)
  {
    if(readonly)
      throw interpreter_exception(get_name() + " is readonly variable");

    if(is_null)
        value.erase(index);
    else
        value[index] = new_value;
  }

  /// \brief get the length of a variable
  /// \param the index of the variable, use 0 if it's not an array
  /// \return the length of the variable
  unsigned get_length(const unsigned index=0) const
  {
    return get_value<std::string>(index).size();
  }

  /// \brief get the length of an array variable
  /// \return the length of the array
  unsigned get_array_length() const
  {
    return value.size();
  }

  /// \brief check whether the value of the variable is null
  /// \return whether the value of the variable is null
  bool is_null(const unsigned index=0) const
  {
    return value.find(index) == value.end();
  }
};

// specialization for arrays
template <>
inline variable::variable<>(const std::string& name,
                            const std::map<int, std::string>& v,
                            bool ro,
                            bool, unsigned)
    : name(name), value(v.begin(), v.end()), readonly(ro)
{
}

///
/// class scope
/// \brief implementation for symbol table
///
class scope
{
public:
  typedef std::unordered_map<std::string, std::shared_ptr<variable>>
    table_type;
  typedef table_type::iterator iterator;
  typedef table_type::const_iterator const_iterator;
  typedef table_type::size_type size_type;
  typedef table_type::value_type value_type;

  ///
  /// \brief return the number of variables in current scope
  /// \return the number of variables
  size_type size()
  {
    return members.size();
  }

  ///
  /// \brief return an iterator referring to the first variable
  /// \return iterator referring to the first variable
  iterator begin()
  {
    return members.begin();
  }

  ///
  /// \brief return a const iterator referring to the first variable
  /// \return const iterator referring to the first variable
  const_iterator begin() const
  {
    return members.begin();
  }

  ///
  /// \brief return an iterator referring to the next element after
  ///        the last variable in current scope
  /// \return iterator referring to he next element after the last
  ///         variable in current scope
  iterator end()
  {
    return members.end();
  }

  ///
  /// \brief return a const iterator referring to the next element
  ///        after the last variable in current scope
  /// \return const iterator referring to he next element after the
  ///         last variable in current scope
  const_iterator end() const
  {
    return members.end();
  }

  /// \brief define a new variable
  /// \param the new variable
  void define(std::shared_ptr<variable> s)
  {
    members[s->get_name()] = s;
  }

  /// \brief resolve a variable
  /// \param the variable name
  /// \return target variable passed by reference
  std::shared_ptr<variable> resolve(const std::string& name)
  {
    auto iter = members.find(name);
    return (iter == members.end()? std::shared_ptr<variable>() : iter->second);
  }
protected:
  /// \var protected::member
  /// \brief symbol table data structure
  table_type members;
};
#endif
