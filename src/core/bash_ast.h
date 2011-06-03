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
/// \brief a class that helps interpret from istream and string
///

#ifndef LIBBASH_CORE_PARSER_BUILDER_H_
#define LIBBASH_CORE_PARSER_BUILDER_H_

#include <type_traits>
#include <functional>
#include <istream>
#include <memory>
#include <string>
#include <vector>

#include <antlr3.h>
#include <boost/utility.hpp>

#include "libbashWalker.h"

struct libbashLexer_Ctx_struct;
struct libbashParser_Ctx_struct;
class interpreter;

template<typename T>
class antlr_pointer: public std::unique_ptr<T, std::function<void(T*)>>
{
  typedef std::unique_ptr<T, std::function<void(T*)>> parent;
public:
  antlr_pointer(T* p = 0) : parent(p, [](T* to_delete) { to_delete->free(to_delete); }) {};
};

/// \class bash_ast
/// \brief a wrapper class that helps interpret from istream and string
class bash_ast: public boost::noncopyable
{
  antlr_pointer<ANTLR3_INPUT_STREAM_struct> input;
  std::string script;
  antlr_pointer<libbashLexer_Ctx_struct> lexer;
  antlr_pointer<ANTLR3_COMMON_TOKEN_STREAM_struct> token_stream;
  antlr_pointer<libbashParser_Ctx_struct> parser;
  pANTLR3_BASE_TREE ast;
  antlr_pointer<ANTLR3_COMMON_TREE_NODE_STREAM_struct> nodes;
  std::function<pANTLR3_BASE_TREE(libbashParser_Ctx_struct*)> parse;

  void init_parser(const std::string& script, const std::string& script_path);

public:
  bash_ast(const std::istream& source,
           std::function<pANTLR3_BASE_TREE(libbashParser_Ctx_struct*)> p=parser_start);

  bash_ast(const std::string& script_path,
           std::function<pANTLR3_BASE_TREE(libbashParser_Ctx_struct*)> p=parser_start);

  static void walker_start(plibbashWalker tree_parser);

  static int walker_arithmetics(plibbashWalker tree_parser);

  static pANTLR3_BASE_TREE parser_start(libbashParser_Ctx_struct* parser);

  static pANTLR3_BASE_TREE parser_arithmetics(libbashParser_Ctx_struct* parser);

  ///
  /// \brief interpret the script with a given interpreter
  /// \param the interpreter object
  /// \return the interpreted result
  template<typename Functor>
  typename std::result_of<Functor(plibbashWalker)>::type
  interpret_with(interpreter& walker, Functor walk)
  {
    set_interpreter(&walker);
    antlr_pointer<libbashWalker_Ctx_struct> p_tree_parser(libbashWalkerNew(nodes.get()));
    return walk(p_tree_parser.get());
  }

  void interpret_with(interpreter& walker)
  {
    interpret_with(walker, walker_start);
  }

  std::string get_dot_graph();

  std::string get_string_tree();

  std::string get_tokens(std::function<std::string(ANTLR3_UINT32)>);
};

#endif
