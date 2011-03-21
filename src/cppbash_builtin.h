/*
   Copyright 2010 Nathan Eloe

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
/// \author Nathan Eloe
/// \brief Base class for builtin functions in bash
///

#ifndef CPPBASH_BUILTIN_H
#define CPPBASH_BUILTIN_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/functional/factory.hpp>
#include <boost/function.hpp>

#define STREAM_ARGS std::ostream &out, std::ostream &err, std::istream &in

///
/// \class cppbash_builtin
/// \brief a virtual class to inherit builtin functions from
///
class cppbash_builtin
{
  public:
    ///
    /// \brief Default constructor, sets default streams
    /// \param outstream where to send standard output.  Default: cout
    /// \param errstream where to send standard error.  Default: cerr
    /// \param instream where to get standard input from.  Default: stdin
    ///
    cppbash_builtin(STREAM_ARGS);

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

    static int exec(const std::string& builtin, const std::vector<std::string>& args, STREAM_ARGS)
    {
      boost::scoped_ptr<cppbash_builtin> p(builtins()[builtin](out,err,in));
      return p->exec(args);
    }

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
    /// \var builtins
    /// \brief holds factories for creating instances of child classes
    ///
    typedef std::map<std::string, boost::function< cppbash_builtin*(STREAM_ARGS) >> builtins_type;
    static builtins_type& builtins();
};

#define BUILTIN_CONSTRUCTOR(name) \
  name ## _builtin(STREAM_ARGS) : cppbash_builtin(out, err, in) {}

#endif
