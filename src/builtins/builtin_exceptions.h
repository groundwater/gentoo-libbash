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
/// \author Mu Qiao
/// \brief implementations for builtin exceptions
///

#ifndef LIBBASH_BUILTINS_BUILTIN_EXCEPTIONS_H_
#define LIBBASH_BUILTINS_BUILTIN_EXCEPTIONS_H_

#include <stdexcept>

///
/// \class return_exception
/// \brief thrown when executing the return builtin
///
class return_exception: public std::runtime_error
{
public:
  explicit return_exception():
    runtime_error(""){}
};

#endif
