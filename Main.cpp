#include <iostream>
#include "tclap\CmdLine.h"
#include "spdlog\spdlog.h"
#include "FileEncrypter.h"




int main(int argc, char* argv[])
{


	std::shared_ptr<spdlog::logger> LOG = spdlog::stdout_color_mt("GCM_ENC_MAIN");

	try
	{
		TCLAP::CmdLine cmd("GCM File Encrypter \n\n Encrypt files using Galois/Counter Mode (GCM)", ' ', "0.1");
		TCLAP::SwitchArg mod("u", "unlock", "decrypt files", false);
		TCLAP::SwitchArg dir("d", "directory", "process files in a directory", false);
		TCLAP::SwitchArg rec("r", "recursive", "look for files recursively. Must be used in combination with -d. If -d is not specified, the argument is ignored", false);
		TCLAP::ValueArg<std::string> pass("p", "password", "password used for processing", true, "Password", "string");
		TCLAP::UnlabeledMultiArg<std::string> fileArgs("files", "files/folders you want to process", true, "string");

		cmd.add(pass);
		cmd.add(mod);
		cmd.add(rec);
		cmd.add(dir);
		cmd.add(fileArgs);

		cmd.parse(argc, argv);

		std::string password = pass.getValue();
		std::vector<std::string> files = fileArgs.getValue();
		bool recursive = rec.getValue();
		bool directory = dir.getValue();
		bool decryptionMode = mod.getValue();



		std::vector<filesystem::path> ALL_FILES;

		if (directory) {
			for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
				if (!filesystem::is_directory(filesystem::path(*it))) { LOG->warn("SKIPPING {}. Cause : not a directory!", *it); continue; }
				if (!filesystem::exists(filesystem::path(*it))) { LOG->warn("SKIPPING {}. Cause : does not exist!", *it); continue; }

				std::vector<filesystem::path> tmp;
				if (recursive) tmp = fileUtils::listFilesInDirRecursively(it->c_str());
				else tmp = fileUtils::listFilesInDir(it->c_str());
				ALL_FILES.insert(std::end(ALL_FILES), std::begin(tmp), std::end(tmp));
			}
		}
		else {
			for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
			{
				if (filesystem::is_directory(filesystem::path(*it))) { LOG->warn("SKIPPING {}. Cause : file is a directory", *it); continue; }
				if (!filesystem::exists(filesystem::path(*it))) { LOG->warn("SKIPPING {}. Cause : file does not exist", *it); continue; }
				ALL_FILES.push_back(filesystem::path(*it));
			}
		}


		FileEncrypter enc;

		if (decryptionMode) {
			enc.decryptFiles(ALL_FILES, password);
		}
		else {
			enc.encryptFiles(ALL_FILES, password);
		}

		password.erase(password.begin(), password.end());

	}
	catch (const TCLAP::ArgException &e)
	{
		LOG->critical(e.what());
	}

}
