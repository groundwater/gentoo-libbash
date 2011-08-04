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
/// \file inherit_builtin.h
/// \brief class that implements the inherit function from Portage
///

#ifndef LIBBASH_BUILTINS_INHERIT_BUILTIN_H_
#define LIBBASH_BUILTINS_INHERIT_BUILTIN_H_

#include "cppbash_builtin.h"

///
/// \class inherit_builtin
/// \brief the inherit builtin for bash
///
class inherit_builtin: public virtual cppbash_builtin
{
  public:
    BUILTIN_CONSTRUCTOR(inherit)

    ///
    /// \brief runs the inherit builtin on the supplied arguments
    /// \param bash_args the arguments to the inherit builtin
    /// \return exit status of inherit
    ///
    virtual int exec(const std::vector<std::string>& bash_args);
  private:
    void append_global(const std::string& name);

    void backup_global(const std::string& name, std::string& value);

    void restore_global(const std::string& name, const std::string& value);

    bool hasq(const std::string& value, const std::string& name);
};

#endif
