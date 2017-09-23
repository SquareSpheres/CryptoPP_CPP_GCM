#include "EncryptedFile.h"
#include "cereal\archives\binary.hpp"
#include "cereal\types\vector.hpp"
#include <fstream>

EncryptedFile EncryptedFile::readEncryptedFileFromDisk(const char filename[])
{
	try
	{
		std::ifstream ifs(filename);
		cereal::BinaryInputArchive iArchive(ifs);
		EncryptedFile enc;
		iArchive(enc);
		return enc;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
		return EncryptedFile();
	}
}

void EncryptedFile::writeEncryptedFileToDisk(const char filename[], const EncryptedFile & const enc)
{
	try
	{
		std::ofstream ofs(filename, std::ios::binary | std::ios::ate);
		cereal::BinaryOutputArchive oArchive(ofs);
		oArchive(enc);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
	}
}
