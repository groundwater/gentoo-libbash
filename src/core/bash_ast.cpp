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
/// \author Mu Qiao
/// \brief implementation that helps interpret from istream
///

#include "core/bash_ast.h"

#include <sstream>
#include <thread>

#include "core/interpreter_exception.h"
#include "core/interpreter.h"
#include "libbashLexer.h"
#include "libbashParser.h"
#include "libbashWalker.h"

bash_ast::bash_ast(std::istream& source): error_count(0)
{
  std::stringstream stream;
  stream << source.rdbuf();
  script = stream.str();

  input = antlr3NewAsciiStringInPlaceStream(
      reinterpret_cast<pANTLR3_UINT8>(const_cast<char*>(script.c_str())),
      script.size(),
      NULL);

  if(input == NULL)
    throw interpreter_exception("Unable to open file " + script + " due to malloc() failure");

  init_parser();
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

bash_ast::~bash_ast()
{
  psr->free(psr);
  tstream->free(tstream);
  lxr->free(lxr);
  input->close(input);
}

void bash_ast::init_parser()
{
  lxr = libbashLexerNew(input);
  if ( lxr == NULL )
  {
    std::cerr << "Unable to create the lexer due to malloc() failure" << std::endl;
    error_count = 1;
    return;
  }

  tstream = antlr3CommonTokenStreamSourceNew(
      ANTLR3_SIZE_HINT, lxr->pLexer->rec->state->tokSource);
  if (tstream == NULL)
  {
    std::cerr << "Out of memory trying to allocate token stream" << std::endl;
    error_count = 1;
    return;
  }

  psr = libbashParserNew(tstream);
  if (psr == NULL)
  {
    std::cerr << "Out of memory trying to allocate parser" << std::endl;
    error_count = 1;
    return;
  }

  langAST.reset(new libbashParser_start_return(psr->start(psr)));
  langAST->tree->strFactory->newRaw = &locked_newRaw8;
  langAST->tree->strFactory->destroy = &locked_destroy;
  error_count = psr->pParser->rec->getNumberOfSyntaxErrors(psr->pParser->rec);
}

void bash_ast::interpret_with(interpreter& walker)
{
  set_interpreter(&walker);

  auto nodes = antlr3CommonTreeNodeStreamNewTree(langAST->tree, ANTLR3_SIZE_HINT);
  plibbashWalker treePsr = libbashWalkerNew(nodes);
  try
  {
    treePsr->start(treePsr);
  }
  catch(...)
  {
    treePsr->free(treePsr);
    throw;
  }
  treePsr->free(treePsr);
  nodes->free(nodes);
}

std::string bash_ast::get_dot_graph()
{
  auto nodes = antlr3CommonTreeNodeStreamNewTree(langAST->tree, ANTLR3_SIZE_HINT);
  pANTLR3_STRING graph = nodes->adaptor->makeDot(nodes->adaptor, langAST->tree);
  std::string result(reinterpret_cast<char*>(graph->chars));
  nodes->free(nodes);
  return result;
}

std::string bash_ast::get_string_tree()
{
  return std::string(reinterpret_cast<char*>(
        langAST->tree->toStringTree(langAST->tree)->chars));
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

std::string bash_ast::get_tokens(std::function<std::string(ANTLR3_INT32)> token_map)
{
  std::stringstream result;
  int line_counter = 1;

  // output line number for the first line
  result << line_counter++ << "\t";

  pANTLR3_VECTOR token_list = tstream->getTokens(tstream);
  unsigned token_size = token_list->size(token_list);

  for(unsigned i = 0; i != token_size; ++i)
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
