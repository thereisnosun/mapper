#include <iostream>
#include <string>
#include <boost/program_options.hpp>

constexpr static char szVERSION[] = "1.0";
namespace bpo = boost::program_options;	

void printHelp(const bpo::options_description& helpDesc)
{
	std::cout << helpDesc << std::endl;
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
      ("app", bpo::value<std::string>(), "app");


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

