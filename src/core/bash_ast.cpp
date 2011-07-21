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
/// \file bash_ast.cpp
/// \brief a wrapper class that helps interpret from istream and string
///
#include "core/bash_ast.h"

#include <fstream>
#include <sstream>
#include <thread>

#include <boost/algorithm/string/erase.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "core/exceptions.h"
#include "core/interpreter.h"
#include "libbashLexer.h"
#include "libbashParser.h"
#include "libbashWalker.h"

void bash_ast::read_script(const std::istream& source)
{
  std::stringstream stream;
  stream << source.rdbuf();
  script = stream.str();
  boost::algorithm::erase_all(script, "\\\n");
}

bash_ast::bash_ast(const std::istream& source,
                   std::function<pANTLR3_BASE_TREE(plibbashParser)> p): parse(p)
{
  read_script(source);
  init_parser("unknown source");
}

bash_ast::bash_ast(const std::string& script_path,
                   std::function<pANTLR3_BASE_TREE(plibbashParser)> p): parse(p)
{
  std::stringstream stream;
  std::ifstream file_stream(script_path);
  if(!file_stream)
    throw libbash::parse_exception(script_path + " can't be read");

  read_script(file_stream);
  init_parser(script_path);
}

namespace
{
  std::mutex string_mutex;

  pANTLR3_STRING locked_newRaw8(pANTLR3_STRING_FACTORY factory)
  {
    std::lock_guard<std::mutex> l(string_mutex);
    pANTLR3_STRING_FACTORY pristine = antlr3StringFactoryNew();
    pANTLR3_STRING result = pristine->newRaw(factory);
    pristine->close(pristine);
    return result;
  }

  void locked_destroy(pANTLR3_STRING_FACTORY factory, pANTLR3_STRING string)
  {
    std::lock_guard<std::mutex> l(string_mutex);
    pANTLR3_STRING_FACTORY pristine = antlr3StringFactoryNew();
    pristine->destroy(factory, string);
    pristine->close(pristine);
  }
}

void bash_ast::init_parser(const std::string& script_path)
{
  input.reset(antlr3NewAsciiStringInPlaceStream(
    reinterpret_cast<pANTLR3_UINT8>(const_cast<char*>(script.c_str())),
    // We do not support strings longer than the max value of ANTLR3_UNIT32
    boost::numeric_cast<ANTLR3_UINT32>(script.size()),
    NULL));

  if(!input)
    throw libbash::parse_exception("Unable to open file " + script + " due to malloc() failure");

  input->fileName = input->strFactory->newStr(
      input->strFactory,
      reinterpret_cast<pANTLR3_UINT8>(const_cast<char*>(script_path.c_str())));

  lexer.reset(libbashLexerNew(input.get()));
  if(!lexer)
    throw libbash::parse_exception("Unable to create the lexer due to malloc() failure");

  token_stream.reset(antlr3CommonTokenStreamSourceNew(
      ANTLR3_SIZE_HINT, lexer->pLexer->rec->state->tokSource));
  if(!token_stream)
    throw libbash::parse_exception("Out of memory trying to allocate token stream");

  parser.reset(libbashParserNew(token_stream.get()));
  if(!parser)
    throw libbash::parse_exception("Out of memory trying to allocate parser");

  ast = parse(parser.get());
  ast->strFactory->newRaw = &locked_newRaw8;
  ast->strFactory->destroy = &locked_destroy;
  if(parser->pParser->rec->getNumberOfSyntaxErrors(parser->pParser->rec))
    throw libbash::parse_exception("Something wrong happened while parsing");
}

std::string bash_ast::get_dot_graph()
{
  antlr_pointer<ANTLR3_COMMON_TREE_NODE_STREAM_struct> nodes(
    antlr3CommonTreeNodeStreamNewTree(ast, ANTLR3_SIZE_HINT));
  pANTLR3_STRING graph = nodes->adaptor->makeDot(nodes->adaptor, ast);
  return std::string(reinterpret_cast<char*>(graph->chars));
}

std::string bash_ast::get_string_tree()
{
  return std::string(reinterpret_cast<char*>(ast->toStringTree(ast)->chars));
}

namespace
{
  void print_line_counter(std::stringstream& result,
                          pANTLR3_COMMON_TOKEN token,
                          int& line_counter,
                          int pos)
  {
    char* text = reinterpret_cast<char*>(token->getText(token)->chars);
    for(int i = pos; text[i] == '\n'; ++i)
      result << '\n' << line_counter++ << "\t";
  }
}

std::string bash_ast::get_parser_tokens(antlr_pointer<ANTLR3_COMMON_TOKEN_STREAM_struct>& token_stream,
                                        std::function<std::string(ANTLR3_UINT32)> token_map)
{
  std::stringstream result;
  int line_counter = 1;

  // output line number for the first line
  result << line_counter++ << "\t";

  pANTLR3_VECTOR token_list = token_stream->getTokens(token_stream.get());
  unsigned token_size = token_list->size(token_list);

  for(unsigned i = 0u; i != token_size; ++i)
  {
    pANTLR3_COMMON_TOKEN token = reinterpret_cast<pANTLR3_COMMON_TOKEN>
      (token_list->get(token_list, i));
    std::string tokenName = token_map(token->getType(token));

    if(tokenName != "EOL" && tokenName != "COMMENT" && tokenName != "CONTINUE_LINE")
    {
      result << tokenName << " ";
    }
    // Output \n and line number before each COMMENT token for better readability
    else if(tokenName == "COMMENT")
    {
      print_line_counter(result, token, line_counter, 0);
      result << tokenName;
    }
    // Output \n and line number after each CONTINUE_LINE/EOL token for better readability
    // omit the last \n and line number
    else if(i + 1 != token_size)
    {
      result << tokenName;
      print_line_counter(result, token, line_counter, tokenName == "CONTINUE_LINE"? 1 : 0);
    }
  }
  return result.str();
}

std::string bash_ast::get_walker_tokens(std::function<std::string(ANTLR3_UINT32)> token_map)
{
  std::stringstream result;
  antlr_pointer<ANTLR3_COMMON_TREE_NODE_STREAM_struct> nodes(
    antlr3CommonTreeNodeStreamNewTree(ast, ANTLR3_SIZE_HINT));
  pANTLR3_INT_STREAM istream = nodes->tnstream->istream;
  auto istream_size = istream->size(istream);

  for(ANTLR3_UINT32 i = 1; i <= istream_size; ++i)
  {
    ANTLR3_UINT32 token = istream->_LA(istream, boost::numeric_cast<ANTLR3_INT32>(i));
    if(token == 2)
      result << "DOWN ";
    else if(token == 3)
      result << "UP ";
    else
      result << token_map(istream->_LA(istream, boost::numeric_cast<ANTLR3_INT32>(i))) << " ";
  }
  result << std::endl;

  return result.str();
}

void bash_ast::walker_start(plibbashWalker tree_parser)
{
  tree_parser->start(tree_parser);
}

long bash_ast::walker_arithmetics(plibbashWalker tree_parser)
{
  return tree_parser->arithmetics(tree_parser);
}

std::string bash_ast::walker_string_expr(libbashWalker_Ctx_struct* tree_parser)
{
  return tree_parser->string_expr(tree_parser).libbash_value;
}

pANTLR3_BASE_TREE bash_ast::parser_start(plibbashParser parser)
{
  return parser->start(parser).tree;
}

pANTLR3_BASE_TREE bash_ast::parser_arithmetics(plibbashParser parser)
{
  return parser->arithmetics(parser).tree;
}

pANTLR3_BASE_TREE bash_ast::parser_all_expansions(libbashParser_Ctx_struct* parser)
{
  return parser->all_expansions(parser).tree;
}

void bash_ast::call_function(plibbashWalker ctx,
                             ANTLR3_MARKER index)
{
  auto INPUT = ctx->pTreeParser->ctnstream;

  // Initialize the input stream
  auto ISTREAM = ctx->pTreeParser->ctnstream->tnstream->istream;
  ISTREAM->size(ISTREAM);

  // Push function index into INPUT
  // The actual type of ANTLR3_MARKER is ANTLR3_INT32
  INPUT->push(INPUT, boost::numeric_cast<ANTLR3_INT32>(index));
  // Execute function body
  ctx->compound_command(ctx);
}

bash_ast::walker_pointer bash_ast::create_walker(interpreter& walker,
                                                 antlr_pointer<ANTLR3_COMMON_TREE_NODE_STREAM_struct>& nodes)
{
    set_interpreter(&walker);
    walker.push_current_ast(this);

    auto deleter = [&](plibbashWalker tree_parser)
    {
      tree_parser->free(tree_parser);
      walker.pop_current_ast();
    };

    return walker_pointer(libbashWalkerNew(nodes.get()), deleter);
}
