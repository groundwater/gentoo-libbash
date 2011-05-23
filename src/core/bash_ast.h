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
/// \file bash_ast.h
/// \author Mu Qiao
/// \brief a class that helps interpret from istream
///

#ifndef LIBBASH_CORE_PARSER_BUILDER_H_
#define LIBBASH_CORE_PARSER_BUILDER_H_

#include <istream>
#include <memory>
#include <string>
#include <vector>

#include <antlr3.h>

struct libbashLexer_Ctx_struct;
struct libbashParser_Ctx_struct;
struct libbashParser_start_return_struct;
class interpreter;

/// \class bash_ast
/// \brief a wrapper class that helps interpret from istream
class bash_ast
{
  pANTLR3_INPUT_STREAM input;
  std::string script;
  libbashLexer_Ctx_struct* lexer;
  pANTLR3_COMMON_TOKEN_STREAM token_stream;
  libbashParser_Ctx_struct* parser;
  std::unique_ptr<libbashParser_start_return_struct> ast;
  pANTLR3_COMMON_TREE_NODE_STREAM nodes;
  int error_count;

  void init_parser();
public:
  explicit bash_ast(std::istream& source);
  ~bash_ast();

  int get_error_count() const
  {
    return error_count;
  }
  ///
  /// \brief interpret the script with a given interpreter
  /// \param the interpreter object
  void interpret_with(interpreter& walker);
  std::string get_dot_graph();
  std::string get_string_tree();
  std::string get_tokens(std::function<std::string(ANTLR3_INT32)> token_map);
};

#endif
