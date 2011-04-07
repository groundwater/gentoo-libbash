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
/// \file walker_builder.cpp
/// \author Mu Qiao
/// \brief a class that helps build a libbashWalker from istream
///

#include "core/walker_builder.h"

#include "core/interpreter.h"
#include "libbashWalker.h"

walker_builder::walker_builder(pANTLR3_COMMON_TREE_NODE_STREAM nodes): walker(new interpreter)
{
  set_interpreter(walker);
  plibbashWalker treePsr = libbashWalkerNew(nodes);
  treePsr->start(treePsr);
  treePsr->free(treePsr);
}
