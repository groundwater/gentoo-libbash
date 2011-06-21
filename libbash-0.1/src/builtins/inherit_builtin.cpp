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
/// \file inherit_builtin.cpp
/// \brief class that implements the inherit builtin
///

#include "builtins/inherit_builtin.h"

#include <cstdlib>

#include <string>

#include "core/interpreter.h"

inline void inherit_builtin::append_global(const std::string& name)
{
    if(!_walker.is_unset_or_null(name, 0))
      _walker.set_value("E_" + name, _walker.resolve<std::string>("E_" + name) + " " + _walker.resolve<std::string>(name));
}

inline void inherit_builtin::restore_global(const std::string& name, const std::string& value)
{
  if(value != "")
    _walker.set_value(name, value);
  else
    _walker.unset(name);
}

inline void inherit_builtin::backup_global(const std::string& name, std::string& value)
{
  value = _walker.resolve<std::string>(name);
  _walker.unset(name);
}

inline bool inherit_builtin::hasq(const std::string& value, const std::string& name)
{
  const std::string& target = _walker.resolve<std::string>(name);
  return target.find(value) != std::string::npos;
}

// We do not support any QA warning
int inherit_builtin::exec(const std::vector<std::string>& bash_args)
{
  _walker.set_value("ECLASS_DEPTH", _walker.resolve<long>("ECLASS_DEPTH") + 1);

  // find eclass directory
  std::string eclassdir;
  if(getenv("ECLASSDIR"))
    eclassdir = getenv("ECLASSDIR") + std::string("/");
  else
    eclassdir = "/usr/portage/eclass/";

  std::string location;
  std::string export_funcs_var;
  // These variables must be restored before returning
  std::string PECLASS(_walker.resolve<std::string>("ECLASS"));
  std::string prev_export_funcs_var(_walker.resolve<std::string>("__export_funcs_var"));

  std::string B_IUSE;
  std::string B_REQUIRED_USE;
  std::string B_DEPEND;
  std::string B_RDEPEND;
  std::string B_PDEPEND;

  for(auto iter = bash_args.begin(); iter != bash_args.end(); ++iter)
  {
    location = eclassdir + *iter + ".eclass";
    _walker.set_value("ECLASS", *iter);
    export_funcs_var = "__export_functions_" + _walker.resolve<std::string>("ECLASS_DEPTH");
    _walker.unset(export_funcs_var);

    // Portage implementation performs actions for overlays here but we don't do it for now

    backup_global("IUSE", B_IUSE);
    backup_global("REQUIRED_USE", B_REQUIRED_USE);
    backup_global("DEPEND", B_DEPEND);
    backup_global("RDEPEND", B_RDEPEND);
    backup_global("PDEPEND", B_PDEPEND);

    _walker.execute_builtin("source", {location});

    append_global("IUSE");
    append_global("REQUIRED_USE");
    append_global("DEPEND");
    append_global("RDEPEND");
    append_global("PDEPEND");

    restore_global("IUSE", B_IUSE);
    restore_global("REQUIRED_USE", B_REQUIRED_USE);
    restore_global("DEPEND", B_DEPEND);
    restore_global("RDEPEND", B_RDEPEND);
    restore_global("PDEPEND", B_PDEPEND);

    // Portage implementation exports functions here but we don't do it for now

    if(!hasq(*iter, "INHERITED"))
      _walker.set_value("INHERITED", _walker.resolve<std::string>("INHERITED") + " " + *iter);
  }

  _walker.set_value("ECLASS_DEPTH", _walker.resolve<long>("ECLASS_DEPTH") - 1);
  if(_walker.resolve<long>("ECLASS_DEPTH") > 0)
  {
    _walker.set_value("ECLASS", PECLASS);
  }
  else
  {
    _walker.unset("ECLASS");
    _walker.unset("__export_funcs_var");
  }

  return 0;
}
