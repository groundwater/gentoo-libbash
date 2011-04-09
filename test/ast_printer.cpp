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
/// \file ast_printer.cpp
/// \brief helper program to visualize AST
///

#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/program_options.hpp>

#include "core/parser_builder.h"
#include "libbashParser.h"

namespace po = boost::program_options;

static void print_ast(std::istream& input, bool silent, bool dot)
{
  parser_builder parser(input);

  if(silent)
    return;

  if(dot)
    std::cout << parser.get_dot_graph() << std::endl;
  else
    std::cout << parser.get_string_tree() << std::endl;
}

static inline std::string token_mapper(std::unordered_map<ANTLR3_INT32, std::string> token_map,
                                       ANTLR3_INT32 token)
{
  return token_map[token];
}

static void build_token_map(std::unordered_map<ANTLR3_INT32, std::string>& token_map,
                            const std::string& path)
{
  std::ifstream token_file(path);
  std::string line;
  while(std::getline(token_file, line))
  {
    auto equal = line.find("=");
    if(equal != std::string::npos)
      token_map[boost::lexical_cast<ANTLR3_INT32>(line.substr(equal + 1, line.size()))]
        = line.substr(0, equal);
  }
}

static inline void print_token(std::istream& input,
                               const std::string& token_path,
                               bool silent)
{
  if(silent)
    return;

  parser_builder parser(input);
  std::unordered_map<ANTLR3_INT32, std::string> token_map;
  build_token_map(token_map, token_path);
  std::cout << parser.get_tokens(std::bind(&token_mapper,
                                           token_map,
                                           std::placeholders::_1))
  << std::endl;
}

static void print_files(const std::vector<std::string>& files,
                        bool print_name,
                        std::function<void(std::istream&)> printer)
{
  for(auto iter = files.begin(); iter != files.end(); ++iter)
  {
    if(print_name)
      std::cout << "Interpreting " << *iter << std::endl;

    std::ifstream input(iter->c_str());
    printer(input);
  }
}

static inline void print_expression(const std::string& expr,
                                    std::function<void(std::istream&)> printer)
{
  std::istringstream input(expr);
  printer(input);
}

static inline void print_cin(std::function<void(std::istream&)> printer)
{
  printer(std::cin);
}

int main(int argc, char** argv)
{
  std::vector<std::string> files;
  std::string expr;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("files,f", po::value<std::vector<std::string>>()->multitoken(),
     "input scripts. If this option and -e are not specified, "
     "will use standard input")
    ("expr,e", po::value<std::string>(), "one line of script")
    ("dot,d", "print graphviz doc file instead of tree string if -s is not specified")
    ("token,t", po::value<std::string>(), "Print all tokens instead of AST. "
                                          "The argument is the path to libbash.tokens")
    ("name,n", "When using files as input scripts, print out file names")
    ("silent,s", "do not print any AST")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help"))
  {
    std::cout << desc << std::endl;
    return 1;
  }

  std::function<void(std::istream&)> printer;
  if(vm.count("token"))
    printer = std::bind(&print_token,
                        std::placeholders::_1,
                        vm["token"].as<std::string>(),
                        vm.count("silent"));
  else
    printer = std::bind(&print_ast,
                        std::placeholders::_1,
                        vm.count("silent"),
                        vm.count("dot"));

  if(vm.count("files"))
    print_files(vm["files"].as<std::vector<std::string>>(),
                vm.count("name"),
                printer);
  else if(vm.count("expr"))
    print_expression(vm["expr"].as<std::string>(), printer);
  else
    print_cin(printer);
}
