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

#include <boost/numeric/conversion/cast.hpp>

#include "core/interpreter_exception.h"
#include "libbashLexer.h"
#include "libbashParser.h"

bash_ast::bash_ast(const std::istream& source,
                   std::function<pANTLR3_BASE_TREE(plibbashParser)> p): parse(p)
{
  std::stringstream stream;
  stream << source.rdbuf();
  script = stream.str();
  init_parser(script, "unknown source");
}

bash_ast::bash_ast(const std::string& script_path,
                   std::function<pANTLR3_BASE_TREE(plibbashParser)> p): parse(p)
{
  std::stringstream stream;
  std::ifstream file_stream(script_path);
  if(!file_stream)
    throw interpreter_exception(script_path + " can't be read");

  stream << file_stream.rdbuf();
  script = stream.str();
  init_parser(script, script_path);
}

void bash_ast::init_parser(const std::string& script, const std::string& script_path)
{
  input.reset(antlr3NewAsciiStringInPlaceStream(
    reinterpret_cast<pANTLR3_UINT8>(const_cast<char*>(script.c_str())),
    // We do not support strings longer than the max value of ANTLR3_UNIT32
    boost::numeric_cast<ANTLR3_UINT32>(script.size()),
    NULL));

  if(!input)
    throw interpreter_exception("Unable to open file " + script + " due to malloc() failure");

  input->fileName = input->strFactory->newStr(
      input->strFactory,
      reinterpret_cast<pANTLR3_UINT8>(const_cast<char*>(script_path.c_str())));

  lexer.reset(libbashLexerNew(input.get()));
  if(!lexer)
    throw interpreter_exception("Unable to create the lexer due to malloc() failure");

  token_stream.reset(antlr3CommonTokenStreamSourceNew(
      ANTLR3_SIZE_HINT, lexer->pLexer->rec->state->tokSource));
  if(!token_stream)
    throw interpreter_exception("Out of memory trying to allocate token stream");

  parser.reset(libbashParserNew(token_stream.get()));
  if(!parser)
    throw interpreter_exception("Out of memory trying to allocate parser");

  ast = parse(parser.get());
  if(parser->pParser->rec->getNumberOfSyntaxErrors(parser->pParser->rec))
    throw interpreter_exception("Something wrong happened while parsing");
  nodes.reset(antlr3CommonTreeNodeStreamNewTree(ast, ANTLR3_SIZE_HINT));
}

std::string bash_ast::get_dot_graph()
{
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

std::string bash_ast::get_tokens(std::function<std::string(ANTLR3_UINT32)> token_map)
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

void bash_ast::walker_start(plibbashWalker tree_parser)
{
  tree_parser->start(tree_parser);
}

int bash_ast::walker_arithmetics(plibbashWalker tree_parser)
{
  return tree_parser->arithmetics(tree_parser);
}

pANTLR3_BASE_TREE bash_ast::parser_start(plibbashParser parser)
{
  return parser->start(parser).tree;
}

pANTLR3_BASE_TREE bash_ast::parser_arithmetics(plibbashParser parser)
{
  return parser->arithmetics(parser).tree;
}


