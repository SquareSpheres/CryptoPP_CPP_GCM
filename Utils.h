#pragma once
#include <vector>
#include "spdlog\spdlog.h"

namespace fileUtils {

	/// <summary>
	/// Read data from file and return it as a vector. If an 
	/// I/O error occurs, the function returns a empty vector.
	/// </summary>
	/// <param name="filename">File location</param>
	/// <returns>A vector with file bytes</returns>
	std::vector<char> ReadAllBytes(char const* filename);

}

