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
/// \file read_builtin.h
/// \brief class that implements the read builtin
///

#ifndef LIBBASH_BUILTINS_READ_BUILTIN_H_
#define LIBBASH_BUILTINS_READ_BUILTIN_H_

#include "../cppbash_builtin.h"

///
/// \class read_builtin
/// \brief the read builtin for bash
///
class read_builtin: public virtual cppbash_builtin
{
  public:
    BUILTIN_CONSTRUCTOR(read)

    ///
    /// \brief runs the read builtin on the supplied arguments
    /// \param bash_args the arguments to the read builtin
    /// \return exit status of read
    ///
    virtual int exec(const std::vector<std::string>& bash_args);

  private:
    ///
    /// \brief assigns words from input to the variables
    /// \param args the variables to be used
    /// \param input the input to be assigned to the variables
    ///
    virtual void process(const std::vector<std::string>& bash_args, const std::string& input);
};

#endif
