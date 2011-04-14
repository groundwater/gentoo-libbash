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
/// \file interpreter.cpp
/// \author Mu Qiao
/// \brief implementations for bash interpreter (visitor pattern).
///

#include "core/interpreter.h"

#include <boost/algorithm/string/join.hpp>

void interpreter::get_all_elements_joined(const std::string& name,
                                          const std::string& delim,
                                          std::string& result)
{
  std::vector<std::string> source;

  auto i = members.find(name);
  if(i != members.end())
  {
    i->second->get_all_values(source);
    result = boost::algorithm::join(source, delim);
  }
  else
  {
    result = "";
  }
}

void interpreter::get_all_elements(const std::string& name,
                                   std::string& result)
{
  get_all_elements_joined(name, " ", result);
}

void interpreter::get_all_elements_IFS_joined(const std::string& name,
                                              std::string& result)
{
  get_all_elements_joined(name,
                          resolve<std::string>("IFS").substr(0, 1),
                          result);
}
