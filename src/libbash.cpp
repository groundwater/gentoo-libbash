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
/// \file libbash.cpp
/// \author Mu Qiao
/// \brief implementation for libbash interface
///

#include "libbash.h"

#include <antlr3defs.h>

#include "core/interpreter_exception.h"
#include "libbashLexer.h"
#include "libbashParser.h"
#include "libbashWalker.h"

namespace libbash
{

  void interpret(const std::string& path,
                 std::unordered_map<std::string, std::string>& variables)
  {
    pANTLR3_INPUT_STREAM input = antlr3AsciiFileStreamNew(
        reinterpret_cast<pANTLR3_UINT8>(const_cast<char *>(path.c_str())));
    if ( input == NULL )
      throw interpreter_exception("Unable to create input stream for script: " + path);

    plibbashLexer lxr = libbashLexerNew(input);
    if ( lxr == NULL )
      throw interpreter_exception("Unable to create the lexer due to malloc() failure");

    pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(
        ANTLR3_SIZE_HINT, lxr->pLexer->rec->state->tokSource);
    if (tstream == NULL)
      throw interpreter_exception("Out of memory trying to allocate token stream");

    plibbashParser psr = libbashParserNew(tstream);
    if (psr == NULL)
      throw interpreter_exception("Out of memory trying to allocate parser");

    std::shared_ptr<interpreter> walker(new interpreter);
    set_interpreter(walker);

    libbashParser_start_return langAST = psr->start(psr);
    pANTLR3_COMMON_TREE_NODE_STREAM nodes = antlr3CommonTreeNodeStreamNewTree(langAST.tree, ANTLR3_SIZE_HINT);
    plibbashWalker treePsr = libbashWalkerNew(nodes);
    treePsr->start(treePsr);

    treePsr->free(treePsr);
    nodes->free(nodes);
    psr->free(psr);
    tstream->free(tstream);
    lxr->free(lxr);
    input->close(input);

    for(auto iter = walker->begin(); iter != walker->end(); ++iter)
    {
      variables[iter->first]=iter->second->get_value<std::string>();
    }
  }
}
