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
/// \file walker_builder.h
/// \author Mu Qiao
/// \brief a class that helps build a libbashWalker
///

#ifndef WALKER_BUILDER_H
#define WALKER_BUILDER_H

#include <memory>

#include <antlr3.h>

class interpreter;

/// \class walker_builder
/// \brief a wrapper class that creates libbashWalker
class walker_builder
{
public:
  /// \var public::walker
  /// \brief the interpreter object that contains all runtime information
  std::shared_ptr<interpreter> walker;

  walker_builder(pANTLR3_COMMON_TREE_NODE_STREAM nodes);
};

#endif
