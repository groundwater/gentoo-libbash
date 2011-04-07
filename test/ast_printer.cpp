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
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "core/parser_builder.h"
#include "libbashParser.h"

static void print_usage()
{
  std::cout<<
    "Usage:\n"<<
    "-f, --file FILE   using FILE as input script, if this option and -e are\n"
    "                  not specified, will use standard input\n"<<
    "-e, --expr EXPR   using EXPR as input script\n"<<
    "-t, --tree        print tree\n"<<
    "-d, --dot         print graphviz doc file (default)"<< std::endl;
}

int main(int argc, char** argv)
{
  bool dot = true;
  char* path = 0;
  std::string expr;

  for(int i = 1; i < argc; ++i)
  {
    if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file"))
    {
      if(i + 1 == argc)
      {
        print_usage();
        exit(EXIT_FAILURE);
      }
      path = argv[++i];
    }
    else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dot"))
    {
      dot = true;
    }
    else if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--tree"))
    {
      dot = false;
    }
    else if(!strcmp(argv[i], "-e") || !strcmp(argv[i], "--expr"))
    {
      if(i + 1 == argc)
      {
        print_usage();
        exit(EXIT_FAILURE);
      }
      expr = argv[++i];
    }
    else
    {
      print_usage();
      exit(EXIT_FAILURE);
    }
  }

  std::unique_ptr<parser_builder> parser;
  if(path)
  {
    std::ifstream input(path);
    if(!input)
    {
      std::cerr << "Unable to create fstream for " << path << std::endl;
      exit(EXIT_FAILURE);
    }
    parser.reset(new parser_builder(input));
  }
  else if(expr.size())
  {
    std::istringstream input(expr);
    parser.reset(new parser_builder(input));
  }
  else
  {
    parser.reset(new parser_builder(std::cin));
  }

  if(dot)
    std::cout << parser->get_dot_graph() << std::endl;
  else
    std::cout << parser->get_string_tree() << std::endl;
}
