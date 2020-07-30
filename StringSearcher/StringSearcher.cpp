#include <iostream>
#include "Researcher.h"
//Checks if on the start of directory string we have A-Z letter and :\ 
bool isValid(const std::string& directory)
{
	std::regex r("(^[A-Z]:\\\\)");
	return regex_search(directory, r);
}
int main(int argc, char* argv[])
{
	//Put directory path and string in double quotes example:
	//"D:\Visual Studio\file.txt"  "Look for that string"
	std::string directory_path;
	std::string pattern;
	for (int i = 0; i < argc; i++)
	{
		std::cout <<"i :"<<i<<" "<< argv[i] << '\n';
	}
	if (argc > 3)
	{
		std::cout << "Not properly given command line arguments\n";
		return 0;
	}

	directory_path = argv[1];
	pattern = argv[2];
	if (!isValid(directory_path))
	{
		std::cout << "Directory path is not valid\n";
		return 0;
	}
	auto searcher = std::make_unique<Researcher>(directory_path, pattern);
	try {
		searcher->execute();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
	
	return 0;

}



