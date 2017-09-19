#include "Utils.h"

#include <fstream>
#include <iostream>

// Logger
static std::shared_ptr<spdlog::logger> LOG = spdlog::stdout_color_mt("FileUtils");


std::vector<char> fileUtils::ReadAllBytes(char const * filename)
{
	std::ifstream ifs(filename, std::ios::binary | std::ios::ate);

	try
	{
		ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::ifstream::pos_type pos = ifs.tellg();
		std::vector<char>  result(pos);
		ifs.seekg(0, std::ios::beg);
		ifs.read(&result[0], pos);
		return result;
	}
	catch (const std::ios_base::failure& e)
	{
		LOG->critical(e.what());
		return std::vector<char>();
	}
}
