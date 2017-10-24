#include "Utils.h"
#include <fstream>

// Logger
static std::shared_ptr<spdlog::logger> LOG = spdlog::stdout_color_mt("FileUtils");


namespace filesystem = std::experimental::filesystem::v1;


std::vector<unsigned char> fileUtils::ReadAllBytes(char const * filename)
{
	std::ifstream ifs(filename, std::ios::binary | std::ios::ate);

	try
	{
		ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::ifstream::pos_type pos = ifs.tellg();
		std::vector<unsigned char>  result(pos);
		ifs.seekg(0, std::ios::beg);
		ifs.read(reinterpret_cast<char*>(&result[0]), pos);
		ifs.close();
		return result;
	}
	catch (const std::ios_base::failure& e)
	{
		LOG->critical(e.what());
		return std::vector<unsigned char>();
	}
}


void fileUtils::WriteAllBytes(char const * filename, std::vector<unsigned char> data)
{

	std::ofstream ofs(filename, std::ios::binary);

	try
	{
		ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
		ofs.write(reinterpret_cast<char*>(data.data()), data.size());
		ofs.close();

	}
	catch (const std::ios_base::failure& e)
	{
		LOG->critical(e.what());
	}

}

std::vector<filesystem::path> fileUtils::ListFilesInDirRecursively(const char filename[])
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


std::vector<filesystem::path> fileUtils::ListFilesInDir(const char filename[])
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


