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
/// \file libbash.h
/// \author Mu Qiao
/// \brief public interface for libbash
///

#ifndef LIBBASH_LIBBASH_H_
#define LIBBASH_LIBBASH_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "core/interpreter_exception.h"

namespace libbash
{
  ///
  /// \brief interpret a script specifid by path, return a map filled with
  ///        variables defined in the script
  /// \param the path of target script
  /// \param the map to store variables
  void LIBBASH_API interpret(const std::string& path,
                             std::unordered_map<std::string, std::vector<std::string>>& variables,
                             std::vector<std::string>& functions);
}

#endif
