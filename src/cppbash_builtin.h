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
/// \file cppbash_builtin.h
/// \brief Base class for builtin functions in bash
///

#ifndef LIBBASH_CPPBASH_BUILTIN_H_
#define LIBBASH_CPPBASH_BUILTIN_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/functional/factory.hpp>
#include <boost/function.hpp>
#include <boost/utility.hpp>

/// shortcut for the arguments of the constructor
#define BUILTIN_ARGS std::ostream &out, std::ostream &err, std::istream &in, interpreter &walker

class interpreter;
///
/// \class cppbash_builtin
/// \brief a virtual class to inherit builtin functions from
///
class cppbash_builtin: public boost::noncopyable
{
  public:
    ///
    /// \brief Default constructor, sets default streams
    /// \param out where to send standard output.  Default: cout
    /// \param err where to send standard error.  Default: cerr
    /// \param in where to get standard input from.  Default: stdin
    /// \param walker the interpreter object
    ///
    explicit cppbash_builtin(BUILTIN_ARGS);

    virtual ~cppbash_builtin() {};
    ///
    /// \brief executes the code associated with the builtin
    /// \param bash_args arguments passed to the builtin
    /// \return return status of the command
    ///
    virtual int exec(const std::vector<std::string>& bash_args)=0;
    ///
    /// \brief accessor to the std output stream
    /// \return output buffer for the builtin
    ///
    std::ostream& out_buffer() {return *_out_stream;}
    ///
    /// \brief accessor to the std error stream
    /// \return error buffer for the builtin
    ///
    std::ostream& err_buffer() {return *_err_stream;}
    ///
    /// \brief accessor to the std input stream
    /// \return input buffer for the builtin
    ///
    std::istream& input_buffer() {return *_inp_stream;}

    /// \brief execute the given builtin
    /// \param builtin the builtin name
    /// \param args the arguments
    /// \param out where to send standard output.  Default: cout
    /// \param err where to send standard error.  Default: cerr
    /// \param in where to get standard input from.  Default: stdin
    /// \param walker the interpreter object
    /// \return the return status of the builtin
    static int exec(const std::string& builtin,
                    const std::vector<std::string>& args,
                    BUILTIN_ARGS)
    {
      const std::unique_ptr<cppbash_builtin> p(builtins()[builtin](out,err,in,walker));
      return p->exec(args);
    }

    ///
    /// \brief check existence of the builtin
    /// \param builtin builtin name
    /// \return whether it is a builtin
    ///
    static bool is_builtin(const std::string& builtin)
    {
      builtins_type& builtin_map = builtins();
      return builtin_map.find(builtin) != builtin_map.end();
    }

    /// \brief transforms escapes in quoted string
    /// \param string the target string
    /// \param output the place to write
    /// \param ansi_c whether to follow ANSI C standard
    static void transform_escapes(const std::string &string, std::ostream& output, bool ansi_c);

  protected:
    ///
    /// \var *_out_stream
    /// \brief current standard output stream
    ///
    std::ostream *_out_stream;
    ///
    /// \var *_err_stream
    /// \brief current standard error stream
    ///
    std::ostream *_err_stream;
    ///
    /// \var *_inp_stream
    /// \brief current standard input stream
    ///
    std::istream *_inp_stream;

    ///
    /// \var _walker
    /// \brief reference to the interpreter object
    interpreter& _walker;

    /// holds factories for creating instances of child classes
    typedef std::map<std::string, boost::function< cppbash_builtin*(BUILTIN_ARGS) >> builtins_type;
    static builtins_type& builtins();
};

/// shortcut for builtin constructor
#define BUILTIN_CONSTRUCTOR(name) \
  name ## _builtin(BUILTIN_ARGS) : cppbash_builtin(out, err, in, walker) {}

#endif
