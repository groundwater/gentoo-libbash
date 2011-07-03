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
/// \file builtin_exceptions.h
/// \brief implementations for builtin exceptions
///

#ifndef LIBBASH_BUILTINS_BUILTIN_EXCEPTIONS_H_
#define LIBBASH_BUILTINS_BUILTIN_EXCEPTIONS_H_

#include <stdexcept>

#include "core/exceptions.h"

///
/// \class return_exception
/// \brief thrown when executing the return builtin
///
class return_exception: public std::runtime_error
{
public:
  explicit return_exception():
    runtime_error("return exception"){}
};

class loop_control_exception
{
  int count;

  virtual void rethrow() = 0;

protected:
  virtual ~loop_control_exception() {}

public:
  /// \brief the construtor
  /// \param c the nested block counter
  explicit loop_control_exception(int c): count(c) {}

  /// \brief rethrow the exception if the counter is greater than 1
  void rethrow_unless_correct_frame()
  {
    if(count != 1)
    {
      --count;
      rethrow();
    }
  }
};

class continue_exception: public loop_control_exception
{
protected:
  /// \brief rethrow the exception itself
  virtual void rethrow()
  {
    throw *this;
  }

public:
  /// \brief the constructor of the exception
  /// \param c the nested block counter
  explicit continue_exception(int c): loop_control_exception(c) {
    if(c < 1)
      throw libbash::illegal_argument_exception("continue: argument should be greater than or equal to 1");
  }
};

class break_exception: public loop_control_exception
{
protected:
  /// \brief rethrow the exception itself
  virtual void rethrow()
  {
    throw *this;
  }

public:
  /// \brief the constructor of the exception
  /// \param c the nested block counter
  explicit break_exception(int c): loop_control_exception(c) {
    if(c < 1)
      throw libbash::illegal_argument_exception("break: argument should be greater than or equal to 1");
  }
};

class suppress_output: public std::exception
{
};
#endif
