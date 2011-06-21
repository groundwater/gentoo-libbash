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
/// \file divide_by_zero_error.h
/// \brief implementation for divide_by_zero_error
///

#ifndef LIBBASH_CORE_DIVIDE_BY_ZERO_ERROR_H_
#define LIBBASH_CORE_DIVIDE_BY_ZERO_ERROR_H_

#include <stdexcept>
#include <string>

#include "common.h"
#include "interpreter_exception.h"

namespace libbash
{
  ///
  /// \class divide_by_zero_error
  /// \brief exception for dividing by zero error
  ///
  class LIBBASH_API divide_by_zero_error: public libbash::interpreter_exception
  {
  public:
    explicit divide_by_zero_error(const std::string& err_msg):
      libbash::interpreter_exception(err_msg){}
  };
}

#endif
