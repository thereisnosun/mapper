#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <sys/stat.h>

#include "mapper.h"

constexpr static char szVERSION[] = "1.0";
constexpr static size_t MAX_FILESIZE = 20 * 1024 * 1024;
namespace bpo = boost::program_options;	

void printHelp(const bpo::options_description& helpDesc)
{
	std::cout << helpDesc << std::endl;
}


//TODO: create for these checks object validator
bool checkIfExecutable(const std::string& appPath)
{
	struct stat sb;
	return (stat(appPath.c_str(), &sb) == 0 && sb.st_mode & S_IXUSR);
}

bool checkFileSize(const std::string& appPath)
{
	struct stat st;
	stat(appPath.c_str(), &st);
	return st.st_size < MAX_FILESIZE;
}

int main(int argc, char *argv[])
{
	//TODO:

	/*1. Check if executable
	2. Check if size > 20M.
	3. Check if DEBUG.
	*/
	
	bpo::options_description desc{"Options"};
    desc.add_options()
      ("help,h", "Help screen")
      ("version", "Print version")
      ("app", bpo::value<std::string>(), "Path to executable you want to examine");


    if (argc <= 1)
	{
		printHelp(desc);
		return 1;
	}

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
	bpo::notify(vm);

    if (vm.count("app"))
    {
		std::cout << "App name is : " << vm["app"].as<std::string>() << std::endl;
		auto appName = vm["app"].as<std::string>();
		if (!checkIfExecutable(appName))
		{
			std::cout << "Error! File " << appName << " is not executable\n";
			return 1;
		}

		if (!checkFileSize(appName))
		{
			std::cout << "Error! File " << appName << " is bigger then 20Mb\n";
			return 1;
		}
		return mainForMapper(argc, appName.c_str());
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


	return 0;
}

