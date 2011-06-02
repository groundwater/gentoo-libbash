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
/// \file eval_builtin.h
/// \brief class that implements the eval builtin
///

#ifndef LIBBASH_BUILTINS_eval_BUILTIN_H_
#define LIBBASH_BUILTINS_eval_BUILTIN_H_

#include "cppbash_builtin.h"

///
/// \class eval_builtin
/// \brief the eval builtin for bash
///
class eval_builtin: public virtual cppbash_builtin
{
  public:
    BUILTIN_CONSTRUCTOR(eval)

    ///
    /// \brief runs the eval builtin on the supplied arguments
    /// \param bash_args the arguments to the eval builtin
    /// \return exit status of eval
    ///
    virtual int exec(const std::vector<std::string>& bash_args);
};

#endif
