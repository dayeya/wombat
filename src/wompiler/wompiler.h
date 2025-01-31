#ifndef _WOMPILER_H 
#define _WOMPILER_H 

#include <wompiler.h>
#include <lexer.h>

#include <optional>
#include <filesystem>
#include <string>
#include <iostream>

class Wompiler
{
public:
	/**
	 * @brief creates the compiler. defaults the cud to thr cwd.
	 */
	Wompiler() {
		this->SetOutputDirectory(std::filesystem::current_path());
	};

	/**
	 * @brief called when the compiler finishes an execution. flushes the memory.
	 */
	~Wompiler() {};

	/**
	 * @brief Builds the current configurated source_file.
	 */
	void Build() {};

	/**
	 * @brief parses the cmd arguments for the compiler.
	 *		  displays the help menu uppon parsing failure.
	 * @param argc - number of given arguments
	 * @param argv - pointer to the array of given arguments.
	 */
	void ParseArgs(int argc, char** argv);

	/**
	 * @brief sets the current output directory for the executable.
	 *		  *defaults* to the cwd.
	 */
	void SetOutputDirectory(std::filesystem::path p);

	/**
	 * @brief getter for the cud.
	 * @return returns the current output dir.
	 */
	std::filesystem::path GetOutputDirectory();

private:
	const std::string language = "Wombat";
	const std::string compiler_name = "wompiler";
	const std::string version = "v0.1.0";

	const std::string default_msg =
		"Wombat Programming Language " + version + "\n" +
		"Run" + "\033[36m wompiler --help \033[0m" + "for more information.\n";

	const std::string for_usage = "Usage: wombat [options] [INPUTS]\n";

	const std::vector<std::vector<std::string>> options = {
		{"--help",										 "displays this help menu\n"},
		{"--cud",									     "displays the cud, changes the cud if given.\n"},
		{"--build <source_file> <target_dir | CUD>",     "builds the source file onto the <output_dir>, defaults to cud\n"},
		{"--perf <source_file>",						 "performs the building and execution of the executable.\n"}
	};
	const std::string wombat_build_fin = "wombat build successful.";
	const std::string wombat_perf_fin  = "wombat performance successful.";

	std::string source_file;				// source file to be compiled
	std::filesystem::path output_directory; // default output directory for the executable	

	Lexer& lexer;
};

#endif // _WOMPILER_H
