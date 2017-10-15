#include "FileEncrypter.h"

#include "pwdbased.h"
#include "hkdf.h"
#include "osrng.h"

#include "spdlog/spdlog.h"
#include "IOException.h"
#include "GeneralSecurityException.h"

#include "aes.h"
#include "gcm.h"


// Logger
static std::shared_ptr<spdlog::logger> LOG = spdlog::stdout_color_mt("FileEncrypter");


CryptoPP::SecByteBlock FileEncrypter::getAESKeyAlt(const std::string &password, char salt[])
{
	// get password and salt length
	size_t saltLength = strlen(salt);

	// create a key with specified size
	CryptoPP::SecByteBlock key(CryptoPP::SHA256::DIGESTSIZE);

	// derive key using HMAC-based Extract-and-Expand key derivation function by Krawczyk and Eronen.
	CryptoPP::HKDF<CryptoPP::SHA256> keyDerivationFunction;
	unsigned int bufferSize = keyDerivationFunction.DeriveKey(key, key.size(), reinterpret_cast<const byte*>(password.c_str()), password.length(), reinterpret_cast<const byte*>(salt), saltLength, nullptr, 0);

	// check how many bytes were produces in the key array
	if (bufferSize < 32) LOG->error("number of bytes returned is lower than size of buffer");

	return key;
}

CryptoPP::SecByteBlock FileEncrypter::getAESKey(const std::string &password, char salt[])
{

	// create a key with specified size
	CryptoPP::SecByteBlock key(CryptoPP::SHA256::DIGESTSIZE);

	// derive key using PKCS5 PBKDF2 HMAC
	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> keyDerivationFunction;
	// "purpose byte" is ignored for PBKDF2, and if therefore set to 0
	// "timeInSeconds" is ignored because iteration count is specified
	unsigned int achievedItCount = keyDerivationFunction.DeriveKey(key, key.size(), 0, reinterpret_cast<const byte*>(password.c_str()),
		password.length(), reinterpret_cast<const byte*>(salt), FileEncrypter::SALT_LENGTH, FileEncrypter::KDF_ITERATION_COUNT, 0);

	if (achievedItCount < FileEncrypter::KDF_ITERATION_COUNT) LOG->error("achieved iteration count is lower than specified iteration count");

	return key;
}

/*
Replace AutoSeededRandomPool with something more secure/random.

From the cryptoPP wiki pages : https://www.cryptopp.com/wiki/RandomNumberGenerator

"Finally, the library does not currently offer the generators specified by NIST SP800-90A,
Recommendation for Random Number Generation Using Deterministic Random Bit Generators.
This is important because the ANSI X9.17 and X9.31 generators are in deprecated status,
and must not be used after 2015. See SP800-131 for details. In the interim, use NonblockingRng
or OS_GenerateRandomBlock on Windows. Both wrap the the Operating System's generator."

"If you are using an AutoSeeded* generator, then the library will attempt to seed the generator
for you using the underlying OS's entropy pool by way of OS_GenerateRandomBlock."

*/
void FileEncrypter::generateRandomIV(byte * const iv, unsigned int ivSize)
{
	//initialize new random for each function call
	CryptoPP::AutoSeededRandomPool random;
	//fill array with random bytes
	random.GenerateBlock(iv, ivSize);
}

/*
Replace AutoSeededRandomPool with something more secure/random.

From the cryptoPP wiki pages : https://www.cryptopp.com/wiki/RandomNumberGenerator

"Finally, the library does not currently offer the generators specified by NIST SP800-90A,
Recommendation for Random Number Generation Using Deterministic Random Bit Generators.
This is important because the ANSI X9.17 and X9.31 generators are in deprecated status,
and must not be used after 2015. See SP800-131 for details. In the interim, use NonblockingRng
or OS_GenerateRandomBlock on Windows. Both wrap the the Operating System's generator."

"If you are using an AutoSeeded* generator, then the library will attempt to seed the generator
for you using the underlying OS's entropy pool by way of OS_GenerateRandomBlock."

*/
void FileEncrypter::generateRandomSalt(byte * const salt, unsigned saltSize)
{
	//initialize new random for each function call
	CryptoPP::AutoSeededRandomPool random;
	//fill array with random bytes
	random.GenerateBlock(salt, saltSize);
}

filesystem::path FileEncrypter::generateEncryptionName(filesystem::path originalFile)
{
	filesystem::path originalFileName = (originalFile.filename() += ".enc").string();
	filesystem::path newPath = originalFile.remove_filename() /= originalFileName;

	int count = 0;
	while (filesystem::exists(newPath)) {
		// prepend counting number
		newPath = originalFile.remove_filename() /= originalFileName.string().insert(0, std::to_string(count++));
	}

	return newPath;
}

filesystem::path FileEncrypter::generateDecryptionName(filesystem::path encryptedFile)
{
	filesystem::path originalFileName = encryptedFile.filename();
	// remove .enc extension if exist
	if (encryptedFile.has_extension() && encryptedFile.extension() == ".enc") {
		originalFileName.replace_extension("");
	}

	filesystem::path newPath = encryptedFile.remove_filename() /= originalFileName;

	int count = 0;
	while (filesystem::exists(encryptedFile))
	{
		newPath = encryptedFile.remove_filename() /= originalFileName.string().insert(0, std::to_string(count++));
	}

	return newPath;
}

std::vector<filesystem::path> FileEncrypter::encryptFiles(std::vector<filesystem::path> files, const std::string &password)
{

	//create new success vector
	std::vector<filesystem::path> successfullyEncrypted;
	// generate new salt
	byte salt[FileEncrypter::SALT_LENGTH];
	FileEncrypter::generateRandomSalt(salt, FileEncrypter::SALT_LENGTH);
	// generate new AES key
	CryptoPP::SecByteBlock key = getAESKey(password, reinterpret_cast<char*>(salt));

	for (auto it = files.begin(); it != files.end(); ++it)
	{

		// check if file exist
		if (!filesystem::exists(*it)) {
			LOG->warn("Skipping {}, Cause : file does not exist", it->string());
			continue;
		}
		// check if folder
		if (filesystem::is_directory(*it)) {
			LOG->warn("Skipping {}, Cause : file is a directory", it->string());
			continue;
		}
		// check if file a valid file
		if (!filesystem::is_regular_file(*it)) {
			LOG->warn("Skipping {}, Cause : file is not a valid file", it->string());
			continue;
		}
		// check if file size is 0
		if (filesystem::is_empty(*it)) {
			LOG->warn("Skipping {}, Cause : file size is 0", it->string());
			continue;
		}

		// read file data
		std::vector<byte> dataVector = fileUtils::ReadAllBytes(it->string().c_str());
		// generate new IV
		byte iv[FileEncrypter::IV_LENGTH];
		FileEncrypter::generateRandomIV(iv, FileEncrypter::IV_LENGTH);
		// encrypt data
		std::vector<byte> encryptedData = cipherData(key, iv, dataVector);
		// create EncrpytedFile
		EncryptedFile encryptedFile(encryptedData, std::vector<byte>(iv, iv + sizeof(iv)), std::vector<byte>(salt, salt + sizeof(salt)), std::vector<byte>());
		// generate new file path
		filesystem::path newFilePath = FileEncrypter::generateEncryptionName(*it);


		//try to write encrypted file to new path
		try
		{
			EncryptedFile::writeEncryptedFileToDisk(newFilePath.string(), encryptedFile);
			LOG->info("{}/{}  {} encrypted to {}", (it - files.begin()) + 1, files.size(), it->string(), newFilePath.string());
			successfullyEncrypted.push_back(*it);

		}
		catch (const IOException &e)
		{
			LOG->critical("Failed to write {} to disk",newFilePath.string());
			throw;
			//TODO handle exception here
		}
	}

	return successfullyEncrypted;
}

std::vector<filesystem::path> FileEncrypter::encryptFiles(char ** files, const size_t num_files, const std::string &password)
{
	// create vector from 2d array
	std::vector<filesystem::path> paths(num_files);
	for (size_t i = 0; i < num_files; i++)
	{
		paths[i] = filesystem::path(files[i]);
	}

	return encryptFiles(paths, password);
}

std::vector<filesystem::path> FileEncrypter::decryptFiles(std::vector<filesystem::path> files, const std::string &password)
{

	// create new success vector
	std::vector<filesystem::path> successfullyDecrypted;

	for (auto it = files.begin(); it != files.end(); ++it) {

		//try to read encrypted file from disk
		try
		{
			EncryptedFile encryptedFile = EncryptedFile::readEncryptedFileFromDisk(it->string());

			// get values from encrypted file
			const std::vector<byte> *encData = encryptedFile.getData();
			const std::vector<byte> *iv = encryptedFile.getIV();
			const std::vector<byte> *salt = encryptedFile.getSalt();
			const std::vector<byte> *aad = encryptedFile.getAAD();

			// tedious cast (use c-style cast instead?)
			const char * saltConstPtr = reinterpret_cast<const char*>(salt->data());
			char * saltPtr = const_cast<char*>(saltConstPtr);

			// generate AES key
			CryptoPP::SecByteBlock key = getAESKey(password, saltPtr);
			// decrypt data
			std::vector<byte> decryptedData;
			try { decryptedData = decipherData(key, iv->data(), *encData); }
			catch (const GeneralSecurityException &ge) { LOG->critical(ge.what());	continue; }
			// get new name for decrypted file
			filesystem::path newFilePath = FileEncrypter::generateDecryptionName(*it);
			// write data to new file
			fileUtils::WriteAllBytes(newFilePath.string().c_str(), decryptedData);
			// log result
			LOG->info("{} decrypted to {}", it->string(), newFilePath.string());
			// add to success list
			successfullyDecrypted.push_back(*it);

		}
		catch (const IOException &e)
		{
			LOG->warn(e.what());
			continue;
		}
	}

	return std::vector<filesystem::path>();
}

std::vector<filesystem::path> FileEncrypter::decryptFiles(char ** files, const size_t num_files, const std::string &password)
{
	return std::vector<filesystem::path>();
}

std::vector<byte> FileEncrypter::decipherData(CryptoPP::SecByteBlock &key, const byte iv[], const std::vector<byte> &encryptedData)
{

	// array for decrypted data
	std::vector<byte> decryptedData(encryptedData.size());

	// get cipher
	CryptoPP::GCM<CryptoPP::AES>::Decryption decryptor;
	decryptor.SetKeyWithIV(key, key.size(), iv, FileEncrypter::IV_LENGTH);

	// try to decrypt
	try
	{
		// ArraySource --> Decryption filter --> ArraySink
		CryptoPP::ArraySource(&encryptedData[0], encryptedData.size(), true,
			new CryptoPP::AuthenticatedDecryptionFilter(decryptor,
				new CryptoPP::ArraySink(&decryptedData[0], decryptedData.size()), CryptoPP::AuthenticatedDecryptionFilter::DEFAULT_FLAGS, FileEncrypter::GCM_TAG_LENGTH));
	}
	catch (CryptoPP::HashVerificationFilter::HashVerificationFailed& e)
	{
		LOG->critical(e.what());
		throw GeneralSecurityException(e.what());
	}
	catch (CryptoPP::InvalidArgument& e)
	{
		LOG->critical(e.what());
	}
	catch (CryptoPP::Exception& e)
	{
		LOG->critical(e.what());
	}

	return decryptedData;
}

std::vector<byte> FileEncrypter::cipherData(CryptoPP::SecByteBlock &key, const byte iv[], const byte data[], const size_t dataLength)
{
	return cipherData(key, iv, std::vector<byte>(data, data + dataLength));
}

std::vector<byte> FileEncrypter::cipherData(CryptoPP::SecByteBlock &key, const byte iv[], const std::vector<byte> &data)
{
	// create vector for encrytped data
	std::vector<byte> encryptedData(data.size() + CryptoPP::AES::BLOCKSIZE);
	// get cipher
	CryptoPP::GCM<CryptoPP::AES>::Encryption encryptor;
	encryptor.SetKeyWithIV(key, key.size(), iv, FileEncrypter::IV_LENGTH);

	// try to encrypt
	try
	{
		// ArraySource --> Encryption filter --> ArraySink
		CryptoPP::ArraySource(&data[0], data.size(), true,
			new CryptoPP::AuthenticatedEncryptionFilter(encryptor,
				new CryptoPP::ArraySink(&encryptedData[0], encryptedData.size())));
	}
	catch (CryptoPP::Exception& e)
	{
		LOG->critical(e.what());
	}

	return encryptedData;
}


/// <summary>
/// Initializes a new instance of the <see cref="FileEncrypter"/> class.
/// </summary>
FileEncrypter::FileEncrypter()
{
	/*Empty*/
}


/// <summary>
/// Finalizes an instance of the <see cref="FileEncrypter"/> class.
/// </summary>
FileEncrypter::~FileEncrypter()
{
	/*Empty*/
}
