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
/// \file function.h
/// \brief implementation for function
///

#ifndef LIBBASH_CORE_FUNCTION_H_
#define LIBBASH_CORE_FUNCTION_H_

#include <antlr3.h>

#include "exceptions.h"

class bash_ast;
class interpreter;

/// \class function
/// \brief bash function implementation
class function
{
  bash_ast& ast;
  ANTLR3_MARKER index;
public:
  /// \brief the constructor
  /// \param ast_ the reference to the AST
  /// \param i the function index
  function(bash_ast& ast_, ANTLR3_MARKER i): ast(ast_), index(i){}

  /// \brief call the function
  /// \param walker the reference to the interpreter object
  void call(interpreter& walker);
};

#endif
