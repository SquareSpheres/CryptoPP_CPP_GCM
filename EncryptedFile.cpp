#include "EncryptedFile.h"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "spdlog/spdlog.h"
#include "IOException.h"
#include <fstream>


// Logger
static std::shared_ptr<spdlog::logger> LOG = spdlog::stdout_color_mt("EncryptedFile");


EncryptedFile EncryptedFile::readEncryptedFileFromDisk(const std::string &filename)
{

	if (!EncryptedFile::isEncryptedFile(filename)) {
		throw IOException("File is not an instance of EncryptedFile!");
	}

	try
	{
		std::ifstream ifs(filename, std::ios::binary);
		ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		cereal::BinaryInputArchive iArchive(ifs);
		EncryptedFile enc;
		iArchive(enc);
		ifs.close();
		return enc;
	}
	catch (const std::ios_base::failure& e)
	{
		LOG->critical(e.what());
		throw IOException(e.what());
	}
	catch (const cereal::Exception &e) {
		LOG->critical(e.what());
		throw IOException(e.what());
	}
}


void EncryptedFile::writeEncryptedFileToDisk(const std::string &filename, EncryptedFile &enc)
{
	try
	{
		std::ofstream ofs(filename, std::ios::binary);
		ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
		cereal::BinaryOutputArchive oArchive(ofs);
		oArchive(enc);
		ofs.close();
	}
	catch (const std::ios_base::failure& e)
	{
		LOG->critical(e.what());
		throw IOException(e.what());
	}
	catch (const cereal::Exception &e) {
		LOG->critical(e.what());
		throw IOException(e.what());
	}
}

bool EncryptedFile::isEncryptedFile(const std::string & filename)
{
	//TODO
	return true;
}
