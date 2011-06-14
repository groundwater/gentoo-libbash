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
/// \file source_builtin.h
/// \brief class that implements the source builtin
///

#include "builtins/source_builtin.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "builtins/builtin_exceptions.h"
#include "cppbash_builtin.h"
#include "core/interpreter.h"
#include "core/interpreter_exception.h"
#include "core/bash_ast.h"

int source_builtin::exec(const std::vector<std::string>& bash_args)
{
  static std::unordered_map<std::string, std::shared_ptr<bash_ast>> ast_cache;

  if(bash_args.size() == 0)
    throw libbash::interpreter_exception("should provide one argument for source builtin");

  // we need fix this to pass extra arguments as positional parameters
  const std::string& path = bash_args[0];

  auto stored_ast = ast_cache.find(path);
  if(stored_ast == ast_cache.end())
  {
    // ensure the path is cached
    auto iter = ast_cache.insert(make_pair(path, std::shared_ptr<bash_ast>()));
    // this may throw exception
    iter.first->second.reset(new bash_ast(path));
    stored_ast = iter.first;
  }
  else if(!(stored_ast->second))
  {
    throw libbash::interpreter_exception(path + " cannot be fully parsed");
  }

  const std::string& original_path = _walker.resolve<std::string>("0");
  try
  {
    _walker.define("0", path, true);
    stored_ast->second->interpret_with(_walker);
  }
  catch(return_exception& e) {}

  _walker.define("0", original_path, true);

  return _walker.get_status();
}
