#pragma once

#include <string>
#include "secblock.h"
#include "EncryptedFile.h"
#include "Utils.h"

namespace filesystem = std::experimental::filesystem::v1;

class FileEncrypter
{

private:

	/// <summary>
	/// Derive a key using HMAC-based Extract-and-Expand key derivation function by Krawczyk and Eronen.
	/// </summary>
	/// <param name="password">The password.</param>
	/// <param name="salt">The salt.</param>
	/// <returns>A key</returns>
	CryptoPP::SecByteBlock getAESKeyAlt(char password[], char salt[]);


	/// <summary>
	/// Derive a key using PKCS5 PBKDF2 HMAC
	/// </summary>
	/// <param name="password">The password.</param>
	/// <param name="salt">The salt.</param>
	/// <returns>A key</returns>
	CryptoPP::SecByteBlock getAESKey(char password[], char salt[]);


	/// <summary>
	/// Generates a random initialization vector
	/// </summary>
	/// <param name="iv">Pointer to iv array.</param>
	/// <param name="ivSize">Size of the iv.</param>
	void generateRandomIV(byte * const iv, unsigned int ivSize);


	/// <summary>
	/// Generates a random salt
	/// </summary>
	/// <param name="salt">Pointer to the salt array.</param>
	/// <param name="saltSize">Size of the salt.</param>
	void generateRandomSalt(byte * const salt, unsigned saltSize);


	/// <summary>
	/// Generates a new name used for the ecnrypted file. The name will be the original name + ".enc" suffix.
	/// If that name already exist, a prefix number will be added as well. E.g. 0myfile.mp3.enc
	/// </summary>
	/// <param name="originalFile">The original file.</param>
	/// <returns>A new unique name</returns>
	filesystem::path generateEncryptionName(filesystem::path originalFile);

	/// <summary>
	/// Generates a new name used for the decrypted file. The new name will simply be the encrypted filename
	/// minus the .enc extension. If the file does not have an extension, a number value will be appended to
	/// the filename to make it unique.
	/// </summary>
	/// <param name="encryptedFile">The encrypted file.</param>
	/// <returns>A unique name</returns>
	filesystem::path generateDecryptionName(filesystem::path encryptedFile);


public:
	const static unsigned int IV_LENGTH = 32; //bytes
	const static unsigned int SALT_LENGTH = 16; //bytes
	const static unsigned int GCM_TAG_LENGTH = 16;//bytes
	const static unsigned int KDF_ITERATION_COUNT = 10000;

	/// <summary>
	/// Encrypts one or many files. The vector can contain one, or many files. The files can be files, or folders.
	/// If its a folder, every file in the folder, including all sub folders will be encrypted.
	/// </summary>
	/// <param name="files">A vector of <see cref="std::experimental::filesystem::v1::path"/></param>
	/// <param name="password">The password.</param>
	/// <returns>
	/// A vector of the files that were successfully encrypted
	/// </returns>
	std::vector<filesystem::path> encryptFiles(std::vector<filesystem::path> files, char password[]);

	/// <summary>
	/// Encrypts one or many files. The array can contain one, or many files. The files can be files, or folders.
	/// If its a folder, every file in the folder, including all sub folders will be encrypted.
	/// </summary>
	/// <param name="files">The files.</param>
	/// <param name="num_files">Number of files.</param>
	/// <param name="password">The password.</param>
	/// <returns>
	/// A vector of the files that were successfully encrypted
	/// </returns>
	std::vector<filesystem::path> encryptFiles(char **files, const size_t num_files, char password[]);

	/// <summary>
	/// Decrypts one or many files. The vector can contain one, or many files. The files can be files, or folders.
	/// If its a folder, every file in the folder, including all sub folders will be decrypted.
	/// </summary>
	/// <param name="files">A vector of <see cref="std::string" /> representing the path to the file</param>
	/// <param name="password">The password.</param>
	/// <returns>
	/// A vector of the files that were successfully decrypted
	/// </returns>
	std::vector<filesystem::path> decryptFiles(std::vector<filesystem::path> files, char password[]);

	/// <summary>
	/// Decrypts one or many files. The array can contain one, or many files. The files can be files, or folders.
	/// If its a folder, every file in the folder, including all sub folders will be decrypted.
	/// </summary>
	/// <param name="files">The files.</param>
	/// <param name="num_files">Number of files.</param>
	/// <param name="password">The password.</param>
	/// <returns>
	/// A vector of the files that were successfully decrypted
	/// </returns>
	std::vector<filesystem::path> decryptFiles(char **files, const size_t num_files, char password[]);



	std::vector<byte> decipherData(CryptoPP::SecByteBlock &key, const byte iv[], const std::vector<byte> &encryptedData);
	std::vector<byte> cipherData(CryptoPP::SecByteBlock &key, const byte iv[], const byte data[], const size_t dataLength);
	std::vector<byte> cipherData(CryptoPP::SecByteBlock &key, const byte iv[], const std::vector<byte> &data);

	void testMethod();



	FileEncrypter();
	~FileEncrypter();
};










