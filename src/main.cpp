#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#ifdef __linux__
#include <sys/stat.h>
#elif _WIN32
    // windows code goes here
#endif

#include "Mapper.h"

constexpr static char szVERSION[] = "1.0";
constexpr static size_t MAX_FILESIZE = 20 * 1024 * 1024;
namespace bpo = boost::program_options;	

void printHelp(const bpo::options_description& helpDesc);
bool checkIfFileOk(const std::string& appPath);

int main(int argc, char *argv[])
{
	bpo::options_description desc{"Options"};
    desc.add_options()
      ("help,h", "Help screen")
      ("version", "Print version")
      ("app", bpo::value<std::string>(), "Path to executable you want to examine(must be built in DEBUG mode)");


    if (argc <= 1)
	{
		printHelp(desc);
		return 1;
	}

	try
	{
		bpo::variables_map vm;
	    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
		bpo::notify(vm);

	    if (vm.count("app"))
	    {
			std::cout << "Will analyze " << vm["app"].as<std::string>() << " applciation" << std::endl;
			std::string appName = vm["app"].as<std::string>();
			if (!checkIfFileOk(appName))
				return 1;

			Mapper mapper{std::move(appName)};
			mapper.collectFunctions();
			mapper.print();
	    }
		else if (vm.count("help"))
		{
			printHelp(desc);
		}
		else if (vm.count("version"))
		{
			std::cout << szVERSION << std::endl;
		}
	    else
	    {
	    	printHelp(desc);
	    	return 1;
	    }
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}



	return 0;
}

void printHelp(const bpo::options_description& helpDesc)
{
	std::cout << helpDesc << std::endl;
}

bool checkIfFileOk(const std::string& appName)
{
#ifdef __linux__
	struct stat st;
	if (stat(appName.c_str(), &st) != 0)
	{
		std::cout << "Error! Cannot acces " << appName << " file\n";
		return false;
	}

	if (!(st.st_mode & S_IXUSR))
	{
		std::cout << "Error! File " << appName << " is not executable\n";
		return false;
	}

	if (st.st_size >= MAX_FILESIZE)
	{
		std::cout << "Error! File " << appName << " is bigger then 20Mb\n";
		return false;
	}
	return true;
#elif _WIN32
	return true;
#endif
}
