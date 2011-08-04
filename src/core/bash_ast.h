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

struct libbashLexer_Ctx_struct;
struct libbashParser_Ctx_struct;
struct libbashWalker_Ctx_struct;
class interpreter;

/// \class antlr_pointer
/// \brief customized unique_ptr for antlr objects.
template<typename T>
class antlr_pointer: public std::unique_ptr<T, std::function<void(T*)>>
{
  /// the constructor of base class
  typedef std::unique_ptr<T, std::function<void(T*)>> parent;
public:
  /// \brief constructor of antlr_pointer
  /// \param p the pointer to the antlr objects, it should provide a method called 'free'
  ///          to free the memory
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
  std::function<pANTLR3_BASE_TREE(libbashParser_Ctx_struct*)> parse;

  typedef std::unique_ptr<libbashWalker_Ctx_struct, std::function<void(libbashWalker_Ctx_struct*)>> walker_pointer;

  void read_script(const std::istream& source);
  void init_parser(const std::string& script_path);
  walker_pointer create_walker(interpreter& walker,
                               antlr_pointer<ANTLR3_COMMON_TREE_NODE_STREAM_struct>& nodes);

public:
  /// \brief build AST from istream
  /// \param source input source
  /// \param p the parser rule for building the AST
  bash_ast(const std::istream& source,
           std::function<pANTLR3_BASE_TREE(libbashParser_Ctx_struct*)> p=parser_start);

  /// \brief build AST from string
  /// \param script_path input source
  /// \param p the parser rule for building the AST
  bash_ast(const std::string& script_path,
           std::function<pANTLR3_BASE_TREE(libbashParser_Ctx_struct*)> p=parser_start);

  /// \brief the functor for walker start rule
  /// \param tree_parser the pointer to the tree_parser
  static void walker_start(libbashWalker_Ctx_struct* tree_parser);

  /// \brief the functor for walker arithmetics rule
  /// \param tree_parser the pointer to the tree_parser
  static long walker_arithmetics(libbashWalker_Ctx_struct* tree_parser);

  /// \brief the functor for walker string_expr rule
  /// \param tree_parser the pointer to the tree_parser
  static std::string walker_string_expr(libbashWalker_Ctx_struct* tree_parser);

  /// \brief call a function that is defined in the AST
  /// \param tree_parser the pointer to the tree_parser
  /// \param index the function index
  static void call_function(libbashWalker_Ctx_struct* tree_parser,
                            ANTLR3_MARKER index);

  /// \brief the functor for parser start rule
  /// \param parser the pointer to the parser
  static pANTLR3_BASE_TREE parser_start(libbashParser_Ctx_struct* parser);

  /// \brief the functor for parser arithmetics rule
  /// \param parser the pointer to the parser
  static pANTLR3_BASE_TREE parser_arithmetics(libbashParser_Ctx_struct* parser);

  /// \brief the functor for parser all_expansions rule
  /// \param parser the pointer to the parser
  static pANTLR3_BASE_TREE parser_all_expansions(libbashParser_Ctx_struct* parser);

  /// \brief the functor for parser builtin_variable_definitions rule
  /// \param parser the pointer to the parser
  /// \param local whether to define the variables in local scope
  static pANTLR3_BASE_TREE parser_builtin_variable_definitions(libbashParser_Ctx_struct* parser, bool local);

  ///
  /// \brief interpret the script with a given interpreter
  /// \param walker the interpreter object
  /// \param walk the walker rule to evaluate the AST
  /// \return the interpreted result
  template<typename Functor>
  typename std::result_of<Functor(libbashWalker_Ctx_struct*)>::type
  interpret_with(interpreter& walker, Functor walk)
  {
    antlr_pointer<ANTLR3_COMMON_TREE_NODE_STREAM_struct> nodes(
      antlr3CommonTreeNodeStreamNewTree(ast, ANTLR3_SIZE_HINT));
    walker_pointer p_tree_parser = create_walker(walker, nodes);
    return walk(p_tree_parser.get());
  }

  ///
  /// \brief use the start rule to interpret the script with a given interpreter
  /// \param walker the interpreter object
  void interpret_with(interpreter& walker)
  {
    interpret_with(walker, walker_start);
  }

  /// \brief get the dot graph for the AST
  /// \return the dot graph
  std::string get_dot_graph();

  /// \brief get the string tree for the AST
  /// \return the string tree
  std::string get_string_tree();

  /// \brief get parser tokens from a token stream
  /// \param token_stream the token stream
  /// \param token_mapper function that translates token numbers to token names
  /// \return the parser tokens
  static std::string get_parser_tokens(antlr_pointer<ANTLR3_COMMON_TOKEN_STREAM_struct>& token_stream,
                                       std::function<std::string(ANTLR3_UINT32)> token_mapper);

  /// \brief get walker tokens from current AST
  /// \param token_mapper function that translates token numbers to token names
  /// \return the walker tokens
  std::string get_walker_tokens(std::function<std::string(ANTLR3_UINT32)> token_mapper);
};

#endif
