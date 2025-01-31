#include <iostream>
#include <filesystem>

#include <wompiler.h>

using namespace std;
namespace fs = std::filesystem;

void Wompiler::ParseArgs(int argc, char** argv)
{
	if (argc == 1) {
		std::cout << this->default_msg;
	}
	else { 
		std::string option = argv[1];
		if (option == "--build") {
			std::string source_file = argv[2];
			std::string output_dir = argv[3];
			this->setcud(output_dir);
		}
		if (option == "--cud") {
			std::cout << "cud: " << this->getcud() << std::endl;
		}
		if (option == "--help") {
			std::cout << this->for_usage;
			for (std::vector<std::string> opt : this->options) {
				std::cout << "  " << setw(42) << left << opt[0] << " - " << opt[1];
			}
		}
	}
}

std::filesystem::path Wompiler::GetOutputDirectory()
{
	return this->output_directory;
}

void Wompiler::SetOutputDirectory(std::filesystem::path p)
{
	this->output_directory = p;
}
