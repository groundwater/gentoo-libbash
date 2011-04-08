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
/// \file parser_builder.h
/// \author Mu Qiao
/// \brief a class that helps build libbashParser from istream
///

#ifndef LIBBASH_CORE_PARSER_BUILDER_H_
#define LIBBASH_CORE_PARSER_BUILDER_H_

#include <istream>
#include <memory>
#include <string>

#include <antlr3.h>

struct libbashLexer_Ctx_struct;
struct libbashParser_Ctx_struct;
struct libbashParser_start_return_struct;
class walker_builder;

/// \class parser_builder
/// \brief a wrapper class that creates libbashParser. It also know
///        enough information to create walker_builder object.
class parser_builder
{
  pANTLR3_INPUT_STREAM input;
  std::string script;
  libbashLexer_Ctx_struct* lxr;
  pANTLR3_COMMON_TOKEN_STREAM tstream;
  libbashParser_Ctx_struct* psr;
  std::shared_ptr<libbashParser_start_return_struct> langAST;
  pANTLR3_COMMON_TREE_NODE_STREAM nodes;

  void init_parser();
public:
  explicit parser_builder(std::istream& source);
  ~parser_builder();

  ///
  /// \brief factory method that creates walker_builder
  /// \return walker_builder object
  walker_builder create_walker_builder();
  std::string get_dot_graph();
  std::string get_string_tree();
};

#endif
