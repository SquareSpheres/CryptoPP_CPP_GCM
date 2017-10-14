#pragma once
#include <vector>
#include <experimental\filesystem>
#include "spdlog/spdlog.h"

namespace fileUtils {


	namespace filesystem = std::experimental::filesystem::v1;

	/// <summary>
	/// Read data from file and return it as a vector. If an 
	/// I/O error occurs, the function returns a empty vector.
	/// </summary>
	/// <param name="filename">File location</param>
	/// <returns>A vector with file bytes</returns>
	std::vector<unsigned char> ReadAllBytes(char const* filename);

	/// <summary>
	/// Write data to file and 
	/// </summary>
	/// <param name="filename">File location</param>
	/// <param name="data">Data to write</param>
	/// <returns></returns>
	void WriteAllBytes(char const* filename, std::vector<unsigned char> data);


	std::vector<filesystem::path> listFilesInDirRecursively(const char filename[]);
	std::vector<filesystem::path> listFilesInDir(const char filename[]);

}

