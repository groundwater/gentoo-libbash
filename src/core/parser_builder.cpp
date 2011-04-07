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
/// \file parser_builder.cpp
/// \author Mu Qiao
/// \brief a class that helps build libbashParser from istream
///

#include "core/parser_builder.h"

#include <sstream>

#include "core/interpreter_exception.h"
#include "libbashLexer.h"
#include "libbashParser.h"
#include "walker_builder.h"

parser_builder::parser_builder(std::istream& source)
{
  std::stringstream stream;
  stream << source.rdbuf();
  script = stream.str();

  input = antlr3NewAsciiStringInPlaceStream(
      reinterpret_cast<pANTLR3_UINT8>(const_cast<char*>(script.c_str())),
      script.size(),
      NULL);
  init_parser();
}

parser_builder::~parser_builder()
{
  nodes->free(nodes);
  psr->free(psr);
  tstream->free(tstream);
  lxr->free(lxr);
  input->close(input);
}

void parser_builder::init_parser()
{
  lxr = libbashLexerNew(input);
  if ( lxr == NULL )
    throw interpreter_exception("Unable to create the lexer due to malloc() failure");

  tstream = antlr3CommonTokenStreamSourceNew(
      ANTLR3_SIZE_HINT, lxr->pLexer->rec->state->tokSource);
  if (tstream == NULL)
    throw interpreter_exception("Out of memory trying to allocate token stream");

  psr = libbashParserNew(tstream);
  if (psr == NULL)
    throw interpreter_exception("Out of memory trying to allocate parser");

  langAST.reset(new libbashParser_start_return(psr->start(psr)));
  nodes = antlr3CommonTreeNodeStreamNewTree(langAST->tree, ANTLR3_SIZE_HINT);
}

walker_builder parser_builder::create_walker_builder()
{
  return walker_builder(nodes);
}
