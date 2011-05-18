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
/// \file bash_condition.cpp
/// \author Mu Qiao
/// \brief implementation for Bash Conditional Expressions
///
#include <ctime>

#include <boost/lexical_cast.hpp>
#include <sys/stat.h>
#include <unistd.h>

#include "core/interpreter_exception.h"

#include "core/bash_condition.h"

namespace
{
  bool test_file_stat(char op, const std::string& path)
  {
    struct stat info;
    int status = 0;

    // symbol link use lstat so we need to separate this.
    if(op == 'L' || op == 'h')
      return lstat(path.c_str(), &info) == 0 && S_ISLNK(info.st_mode);

    status = stat(path.c_str(), &info);
    if(status != 0 || access(path.c_str(), F_OK))
      return false;

    switch(op)
    {
      case 'a':
      case 'e':
        return true;
      case 'b':
        return S_ISBLK(info.st_mode);
      case 'c':
        return S_ISCHR(info.st_mode);
      case 'd':
        return S_ISDIR(info.st_mode);
      case 'f':
        return S_ISREG(info.st_mode);
      case 'g':
        return S_ISGID & info.st_mode;
      case 'k':
        return S_ISVTX & info.st_mode;
      case 'p':
        return S_ISFIFO(info.st_mode);
      case 'r':
        return access(path.c_str(), R_OK) == 0;
      case 's':
        return info.st_size > 0;
      case 'u':
        return S_ISUID & info.st_mode;
      case 'w':
        return access(path.c_str(), W_OK) == 0;
      case 'x':
        return access(path.c_str(), X_OK) == 0;
      case 'O':
        return geteuid() == info.st_uid;
      case 'G':
        return getegid() == info.st_gid;
      case 'S':
        return S_ISSOCK(info.st_mode);
      case 'N':
        return info.st_mtime >= info.st_atime;
      default:
        throw interpreter_exception(std::string("Unrecognized test operator -") + op);
    }
  }
}

bool internal::test_unary(char op, const std::string& target)
{
  switch(op)
  {
    case 'z':
      return target.empty();
    case 'n':
      return !target.empty();
    case 'o':
      throw interpreter_exception("Shell option test is not supported");
    case 't':
      try
      {
        int fd = boost::lexical_cast<int>(target);
        return isatty(fd);
      }
      catch(boost::bad_lexical_cast& e)
      {
        return false;
      }
    default:
      return test_file_stat(op, target);
  }
}

namespace
{
  bool file_comp(char op,
                 const std::string& lhs,
                 const std::string& rhs)
  {
    struct stat lst, rst;
    int lstatus, rstatus;

    lstatus = stat(lhs.c_str(), &lst);
    rstatus = stat(rhs.c_str(), &rst);
    if(op == 'e' && (lstatus < 0 || rstatus < 0))
      return false;

    switch(op)
    {
      case 'n':
        /* -nt */
        return ((lstatus > rstatus) || (lstatus == 0 && lst.st_mtime > rst.st_mtime));
      case 'o':
        /* -ot */
        return ((lstatus < rstatus) || (rstatus == 0 && lst.st_mtime < rst.st_mtime));
      case 'e':
        /* -ef */
        return (lst.st_dev == rst.st_dev && lst.st_ino == rst.st_ino);
      default:
        throw interpreter_exception(std::string("Unrecognized option for file test ") + op);
    }
  }
}

bool internal::test_binary(const std::string& op,
                           const std::string& lhs,
                           const std::string& rhs)
{
  if(op.size() != 2)
    throw interpreter_exception("Unrecognized operator " + op);

  try
  {
    if(op == "nt")
      return file_comp('n', lhs, rhs);
    else if(op == "ot")
      return file_comp('o', lhs, rhs);
    else if(op == "ef")
      return file_comp('e', lhs, rhs);
    // We do not support arithmetic expressions inside keyword test for now.
    // So the operands can only be raw integers.
    else if(op == "eq")
      return boost::lexical_cast<int>(lhs) == boost::lexical_cast<int>(rhs);
    else if(op == "ne")
      return boost::lexical_cast<int>(lhs) != boost::lexical_cast<int>(rhs);
    else if(op == "lt")
      return boost::lexical_cast<int>(lhs) < boost::lexical_cast<int>(rhs);
    else if(op == "le")
      return boost::lexical_cast<int>(lhs) <= boost::lexical_cast<int>(rhs);
    else if(op == "gt")
      return boost::lexical_cast<int>(lhs) > boost::lexical_cast<int>(rhs);
    else if(op == "ge")
      return boost::lexical_cast<int>(lhs) >= boost::lexical_cast<int>(rhs);
    else
      throw interpreter_exception("Unrecognized operator " + op);
  }
  catch(boost::bad_lexical_cast& e)
  {
    return false;
  }
}
