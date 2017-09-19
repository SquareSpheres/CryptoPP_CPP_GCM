#include "FileEncrypter.h"

#include "pwdbased.h"
#include "hkdf.h"
#include "osrng.h"

#include "aes.h"
#include "gcm.h"


// Logger
static std::shared_ptr<spdlog::logger> LOG = spdlog::stdout_color_mt("FileEncrypter");


CryptoPP::SecByteBlock FileEncrypter::getAESKeyAlt(char password[], char salt[])
{
	// get password and salt length
	size_t passwordLength = strlen(password);
	size_t saltLength = strlen(salt);

	// create a key with specified size
	CryptoPP::SecByteBlock key(CryptoPP::SHA256::DIGESTSIZE);

	// derive key using HMAC-based Extract-and-Expand key derivation function by Krawczyk and Eronen.
	CryptoPP::HKDF<CryptoPP::SHA256> keyDerivationFunction;
	unsigned int bufferSize = keyDerivationFunction.DeriveKey(key, key.size(), (const byte*)password, passwordLength, (const byte*)salt, saltLength, NULL, 0);

	// check how many bytes were produces in the key array
	if (bufferSize < 32) LOG->error("number of bytes returned is lower than size of buffer");

	return key;
}


CryptoPP::SecByteBlock FileEncrypter::getAESKey(char password[], char salt[])
{
	// get password and salt length
	size_t passwordLength = strlen(password);
	size_t saltLength = strlen(salt);

	// create a key with specified size
	CryptoPP::SecByteBlock key(CryptoPP::SHA256::DIGESTSIZE);

	// derive key using PKCS5 PBKDF2 HMAC
	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> keyDerivationFunction;
	// "purpose byte" is ignored for PBKDF2, and if therefore set to 0
	// "timeInSeconds" is ignored because iteration count is specified
	unsigned int achievedItCount = keyDerivationFunction.DeriveKey(key, key.size(), 0, (const byte*)password,
		passwordLength, (const byte*)salt, saltLength, FileEncrypter::KDF_ITERATION_COUNT, 0);

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

std::string FileEncrypter::decipherData(char password[], const byte salt[], const byte iv[], const std::string &encryptedData)
{
	// string for decrypted data
	std::string decryptedData;

	// generate key
	CryptoPP::SecByteBlock key = FileEncrypter::getAESKey(password, (char*)salt);

	// get cipher
	CryptoPP::GCM<CryptoPP::AES>::Decryption decryptor;
	decryptor.SetKeyWithIV(key, key.size(), iv, FileEncrypter::IV_LENGTH);

	// try to decrypt
	try
	{
		// StringSource --> Decryption filter --> StringSink
		CryptoPP::StringSource(encryptedData, true,
			new CryptoPP::AuthenticatedDecryptionFilter(decryptor,
				new CryptoPP::StringSink(decryptedData), CryptoPP::AuthenticatedDecryptionFilter::DEFAULT_FLAGS, FileEncrypter::GCM_TAG_LENGTH));
	}
	catch (CryptoPP::HashVerificationFilter::HashVerificationFailed& e)
	{
		LOG->critical(e.what());
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

std::string FileEncrypter::cipherData(char password[], const byte data[], const size_t dataLength)
{
	// generate new salt
	byte salt[FileEncrypter::SALT_LENGTH];
	FileEncrypter::generateRandomSalt(salt, FileEncrypter::SALT_LENGTH);

	// generate new IV
	byte iv[FileEncrypter::IV_LENGTH];
	FileEncrypter::generateRandomIV(iv, FileEncrypter::IV_LENGTH);

	// create string for encrytped data
	std::string encryptedData;

	// generate key
	CryptoPP::SecByteBlock key = FileEncrypter::getAESKey(password, (char*)salt);

	// get cipher
	CryptoPP::GCM<CryptoPP::AES>::Encryption encryptor;
	encryptor.SetKeyWithIV(key, key.size(), iv, FileEncrypter::IV_LENGTH);

	// try to encrypt
	try
	{
		// ArraySource --> Encryption filter --> StringSink
		CryptoPP::ArraySource(data, dataLength, true,
			new CryptoPP::AuthenticatedEncryptionFilter(encryptor,
				new CryptoPP::StringSink(encryptedData), false, FileEncrypter::GCM_TAG_LENGTH));
	}
	catch (CryptoPP::Exception& e)
	{
		LOG->critical(e.what());
	}

	return encryptedData;
}

FileEncrypter::FileEncrypter()
{
}


FileEncrypter::~FileEncrypter()
{
}
