/* This file was partly copied from Paludis src/clients/instruo/instruo.cc.
 * The metadata generation logic is reimplemented with libbash. This file is
 * licensed as follows: */

/*
 * Copyright (c) 2007, 2008, 2009, 2010 Ciaran McCreesh
 *
 * This file is part of the Paludis package manager. Paludis is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * Paludis is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>

#include <paludis/about.hh>
#include <paludis/action.hh>
#include <paludis/args/do_help.hh>
#include <paludis/environment.hh>
#include <paludis/environment_factory.hh>
#include <paludis/filter.hh>
#include <paludis/filtered_generator.hh>
#include <paludis/generator.hh>
#include <paludis/metadata_key.hh>
#include <paludis/name.hh>
#include <paludis/package_id.hh>
#include <paludis/repository.hh>
#include <paludis/selection.hh>
#include <paludis/util/accept_visitor.hh>
#include <paludis/util/destringify.hh>
#include <paludis/util/fs_path.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/join.hh>
#include <paludis/util/log.hh>
#include <paludis/util/make_named_values.hh>
#include <paludis/util/map.hh>
#include <paludis/util/mutex.hh>
#include <paludis/util/pretty_print.hh>
#include <paludis/util/safe_ofstream.hh>
#include <paludis/util/sequence.hh>
#include <paludis/util/set.hh>
#include <paludis/util/system.hh>
#include <paludis/util/thread_pool.hh>
#include <paludis/util/timestamp.hh>
#include <paludis/util/visitor_cast.hh>
#include <paludis/version_spec.hh>

#include "command_line.h"
#include "exceptions.h"
#include "libbash.h"
#include "test.h"
#include "utils/metadata.h"

using namespace paludis;
using std::cout;
using std::cerr;
using std::endl;

void worker(const std::shared_ptr<PackageIDSequence> &ids, const std::string &repository_path)
{
  unsigned total(0);
  CategoryNamePart old_cat("OLDCAT");
  #pragma omp parallel
  {
    #pragma omp single nowait
    while(!ids->empty())
    {
      std::shared_ptr<const PackageID> id = *ids->begin();
      ids->pop_front();
      if (id->name().category() != old_cat)
      {
        std::cout << "Processing " << stringify(id->name().category()) << "..." << std::endl;
        old_cat = id->name().category();
        FSPath(CommandLine::get_instance()->a_output_directory.argument() + "/" +
            stringify(id->name().category())).mkdir(0755,  {fspmkdo_ok_if_exists});
        ++total;
      }

      #pragma omp task
      {
        Context i_context("When generating metadata for ID '" + stringify(*id) + "':");

        std::unordered_map<std::string, std::vector<std::string>> variables;
        variables["PN"].push_back(stringify(id->name().package()));
        variables["PV"].push_back(stringify(id->version().remove_revision()));
        variables["P"].push_back(stringify(id->name().package()) + "-" +
                                 stringify(id->version().remove_revision()));
        variables["PR"].push_back(id->version().revision_only());
        variables["PVR"].push_back(stringify(id->version()));
        variables["PF"].push_back(stringify(id->name().package()) + "-" + stringify(id->version()));
        variables["CATEGORY"].push_back(stringify(id->name().category()));
        std::vector<std::string> functions;

        std::string ebuild_path(repository_path + "/" +
                                variables["CATEGORY"][0] + "/" +
                                variables["PN"][0] + "/" +
                                variables["PN"][0] + "-" +
                                variables["PVR"][0] + ".ebuild");
        try
        {
          libbash::interpret(ebuild_path, get_src_dir() + "/utils/isolated-functions.sh", variables, functions);

          std::string output_path(CommandLine::get_instance()->a_output_directory.argument() + "/" +
                                  variables["CATEGORY"][0] + "/" +
                                  variables["PN"][0] + "-" +
                                  variables["PVR"][0]);
          FSPath(output_path).dirname().mkdir(0755, {fspmkdo_ok_if_exists});
          std::ofstream output(output_path, std::ofstream::out | std::ofstream::trunc);
          write_metadata(output, variables, functions);
        }
        catch(const libbash::interpreter_exception& e)
        {
          cerr << "Exception occurred while interpreting " << ebuild_path << ". The error message is:\n"
            << e.what() << endl;
        }
      }
    }
  }
}


int main(int argc, char** argv)
{
  try
  {
    std::string options(paludis::getenv_with_default("INSTRUO_OPTIONS", ""));
    if (! options.empty())
        options = "(" + options + ") ";
    options += join(argv + 1, argv + argc, " ");

    Context context(std::string("In program ") + argv[0] + " " + options + ":");

    CommandLine::get_instance()->run(argc, argv, "instruo", "INSTRUO_OPTIONS", "INSTRUO_CMDLINE");

    if (CommandLine::get_instance()->a_help.specified())
        throw args::DoHelp();

    if (CommandLine::get_instance()->a_log_level.specified())
        Log::get_instance()->set_log_level(CommandLine::get_instance()->a_log_level.option());
    else
        Log::get_instance()->set_log_level(ll_qa);

    if (1 < (
                CommandLine::get_instance()->a_generate_cache.specified() +
                CommandLine::get_instance()->a_version.specified()
            ))
        throw args::DoHelp("you should specify exactly one action");

    if (! CommandLine::get_instance()->a_package_manager.specified())
        CommandLine::get_instance()->a_package_manager.set_argument("portage");

    if (! CommandLine::get_instance()->a_repository_name.specified())
        CommandLine::get_instance()->a_repository_name.set_argument("gentoo");

    if (CommandLine::get_instance()->a_version.specified())
    {
        cout << "instruo, part of " << PALUDIS_PACKAGE << " " << PALUDIS_VERSION_MAJOR << "."
            << PALUDIS_VERSION_MINOR << "." << PALUDIS_VERSION_MICRO << PALUDIS_VERSION_SUFFIX;
        if (! std::string(PALUDIS_GIT_HEAD).empty())
            cout << " git " << PALUDIS_GIT_HEAD;
        cout << endl << endl;
        cout << "Paludis comes with ABSOLUTELY NO WARRANTY. Paludis is free software, and you" << endl;
        cout << "are welcome to redistribute it under the terms of the GNU General Public" << endl;
        cout << "License, version 2." << endl;

        return EXIT_SUCCESS;
    }

    if (! CommandLine::get_instance()->a_output_directory.specified())
        CommandLine::get_instance()->a_output_directory.set_argument(stringify(FSPath::cwd()));

    FSPath(CommandLine::get_instance()->a_output_directory.argument()).mkdir(0755,  {fspmkdo_ok_if_exists});

    std::shared_ptr<Environment> env(EnvironmentFactory::get_instance()->create(CommandLine::get_instance()->a_package_manager.argument()));

    std::shared_ptr<Repository> repo = env->fetch_repository(RepositoryName(CommandLine::get_instance()->a_repository_name.argument()));

    Repository::MetadataConstIterator distdir_metadata(repo->find_metadata("location"));
    const MetadataValueKey<FSPath> *path_key;
    if(distdir_metadata != repo->end_metadata())
    {
      path_key = visitor_cast<const MetadataValueKey<FSPath>>(**distdir_metadata);

      std::shared_ptr<PackageIDSequence> ids((*env)[selection::AllVersionsSorted(
                  generator::InRepository(RepositoryName(CommandLine::get_instance()->a_repository_name.argument())))]);
      worker(ids, stringify(path_key->parse_value().realpath()));
    }
  }
  catch (const paludis::args::ArgsError & e)
  {
      cerr << "Usage error: " << e.message() << endl;
      cerr << "Try " << argv[0] << " --help" << endl;
      return EXIT_FAILURE;
  }
  catch (const args::DoHelp & h)
  {
      if (h.message.empty())
      {
          cout << "Usage: " << argv[0] << " [options]" << endl;
          cout << endl;
          cout << *CommandLine::get_instance();
          return EXIT_SUCCESS;
      }
      else
      {
          cerr << "Usage error: " << h.message << endl;
          cerr << "Try " << argv[0] << " --help" << endl;
          return EXIT_FAILURE;
      }
  }
  catch (const Exception & e)
  {
      cout << endl;
      cerr << "Unhandled exception:" << endl
          << "  * " << e.backtrace("\n  * ")
          << e.message() << " (" << e.what() << ")" << endl;
      return EXIT_FAILURE;
  }
  catch (const std::exception & e)
  {
      cout << endl;
      cerr << "Unhandled exception:" << endl
          << "  * " << e.what() << endl;
      return EXIT_FAILURE;
  }
  catch (...)
  {
      cout << endl;
      cerr << "Unhandled exception:" << endl
          << "  * Unknown exception type. Ouch..." << endl;
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
