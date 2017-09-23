#include "Utils.h"

#include <fstream>
#include <iostream>



// Logger
static std::shared_ptr<spdlog::logger> LOG = spdlog::stdout_color_mt("FileUtils");


namespace filesystem = std::experimental::filesystem::v1;


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

std::vector<filesystem::path> fileUtils::listFilesInDirRecursively(const char filename[])
{
	std::vector<filesystem::path> files;

	filesystem::path filePath = filename;
	if (!filesystem::exists(filePath) || !filesystem::is_directory(filePath)) {
		LOG->warn("File {} does not exist, or is not a directory", filename);
		return files;
	}

	for (auto &file : filesystem::recursive_directory_iterator(filePath)) {
		if (filesystem::is_regular_file(file)) {
			files.push_back(file.path());
		}
	}

	return files;
}


std::vector<filesystem::path> fileUtils::listFilesInDir(const char filename[])
{
	std::vector<filesystem::path> files;

	filesystem::path filePath = filename;
	if (!filesystem::exists(filePath) || !filesystem::is_directory(filePath)) {
		LOG->warn("File {} does not exist, or is not a directory", filename);
		return files;
	}

	for (auto &file : filesystem::directory_iterator(filePath)) {
		if (filesystem::is_regular_file(file)) {
			files.push_back(file.path());
		}
	}

	return files;
}


