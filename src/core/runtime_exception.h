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
/// \file runtime_exception.h
/// \brief implementation for runtime_exception
///

#ifndef LIBBASH_CORE_RUNTIME_EXCEPTION_H_
#define LIBBASH_CORE_RUNTIME_EXCEPTION_H_

#include <stdexcept>
#include <string>

#include "common.h"
#include "interpreter_exception.h"

namespace libbash
{
  ///
  /// \class runtime_exception
  /// \brief exception for runtime errors
  ///
  class LIBBASH_API runtime_exception: public libbash::interpreter_exception
  {
  public:
    explicit runtime_exception(const std::string& err_msg):
      libbash::interpreter_exception(err_msg){}
  };
}

#endif
