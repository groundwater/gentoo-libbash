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
/// \file metadata.cpp
/// \brief a helper for printing metadata content
///
#include "utils/metadata.h"

#include <set>

#include <boost/spirit/include/karma.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

static const std::vector<std::string> metadata_names = {"DEPEND", "RDEPEND", "SLOT", "SRC_URI",
                                                        "RESTRICT",  "HOMEPAGE",  "LICENSE", "DESCRIPTION",
                                                        "KEYWORDS",  "INHERITED", "IUSE", "REQUIRED_USE",
                                                        "PDEPEND",   "PROVIDE", "EAPI", "PROPERTIES"};

static const std::unordered_map<std::string, std::string> phases = {
  {"pkg_pretend", "ppretend"},
  {"pkg_setup", "setup"},
  {"src_unpack", "unpack"},
  {"src_prepare", "prepare"},
  {"src_configure", "configure"},
  {"src_compile", "compile"},
  {"src_test", "test"},
  {"src_install", "install"},
  {"pkg_preinst", "preinst"},
  {"pkg_postinst", "postinst"},
  {"pkg_prerm", "prerm"},
  {"pkg_postrm", "postrm"},
  {"pkg_config", "config"},
  {"pkg_info", "info"},
  {"pkg_nofetch", "nofetch"}
};

void write_metadata(std::ostream& output,
                    std::unordered_map<std::string, std::vector<std::string>>& variables,
                    std::vector<std::string>& functions)
{
  int EAPI = 0;
  if(variables.find("EAPI") != variables.end())
    EAPI = boost::lexical_cast<int>(variables["EAPI"][0]);
  if(EAPI < 4 && variables.find("RDEPEND") == variables.end() && variables.find("DEPEND") != variables.end())
    variables["RDEPEND"] = variables["DEPEND"];
  for(auto iter_name = metadata_names.begin(); iter_name != metadata_names.end(); ++iter_name)
  {
    auto iter_value = variables.find(*iter_name);
    std::string value;

    if(iter_value != variables.end())
      value = iter_value->second[0];

    // Check if global is defined
    auto iter_global = variables.find("E_" + *iter_name);
    if(iter_global != variables.end())
    {
      boost::trim_if(iter_global->second[0], boost::is_any_of(" \t\n"));
      std::vector<std::string> splitted_global;
      boost::split(splitted_global,
                   iter_global->second[0],
                   boost::is_any_of(" \t\n"),
                   boost::token_compress_on);

      // Append the global value to 'value' if it doesn't cause duplication
      for(auto iter_splitted_global = splitted_global.begin();
          iter_splitted_global != splitted_global.end();
          ++iter_splitted_global)
      {
        if(value.find(*iter_splitted_global) == std::string::npos)
          value += " " + *iter_splitted_global;
      }
    }

    boost::trim_if(value, boost::is_any_of(" \t\n"));

    std::vector<std::string> splitted_value;
    boost::split(splitted_value,
                 value,
                 boost::is_any_of(" \t\n"),
                 boost::token_compress_on);

    // INHERITED eclasses should be sorted
    if(*iter_name == "INHERITED")
      sort(splitted_value.begin(), splitted_value.end());

    using namespace boost::spirit::karma;
    output << format(string % ' ', splitted_value) << std::endl;
  }

  // Print defined phases
  std::set<std::string> sorted_phases;
  for(auto iter = functions.begin(); iter != functions.end(); ++iter)
  {
    auto iter_phase = phases.find(*iter);
    if(iter_phase != phases.end())
      sorted_phases.insert(iter_phase->second);
  }
  using namespace boost::spirit::karma;
  output << format(string % ' ', sorted_phases) << std::endl;

  // Print empty lines
  output << std::endl << std::endl << std::endl << std::endl << std::endl;
}
