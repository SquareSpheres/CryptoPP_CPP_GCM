#pragma once
#include <vector>
#include <assert.h>
#include "cereal\access.hpp"


typedef unsigned char byte;

class EncryptedFile
{

private:
	std::vector<byte> data;
	std::vector<byte> iv;
	std::vector<byte> salt;
	std::vector<byte> aad;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive & ar) {
		ar(data, iv, salt, aad);
	}

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="EncryptedFile"/> class.
	/// </summary>
	/// <param name="data">The encrypted data.</param>
	/// <param name="iv">The initialization vector.</param>
	/// <param name="salt">The salt.</param>
	/// <param name="aad">The additional authenticated data.</param>
	EncryptedFile(std::vector<byte> & const data, std::vector<byte> iv, std::vector<byte> salt, std::vector<byte> aad)
		:aad(aad), salt(salt), iv(iv), data(data) {
		assert(data.size() > 0 && iv.size() > 0 && salt.size() > 0);
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="EncryptedFile"/> class.
	/// </summary>
	EncryptedFile() {};


	/// <summary>
	/// Finalizes an instance of the <see cref="EncryptedFile"/> class.
	/// </summary>
	~EncryptedFile() {}

	/// <summary>
	/// Gets the encrypted data.
	/// </summary>
	/// <returns></returns>
	std::vector<byte> getData() { return this->data; }
	/// <summary>
	/// Gets the iv.
	/// </summary>
	/// <returns></returns>
	std::vector<byte> getIV() { return this->iv; }
	/// <summary>
	/// Gets the salt.
	/// </summary>
	/// <returns></returns>
	std::vector<byte> getSalt() { return this->salt; }
	/// <summary>
	/// Gets the additional authenticated data.
	/// </summary>
	/// <returns></returns>
	std::vector<byte> getAAD() { return this->aad; }

};


namespace fileUtils {

	/// <summary>
	/// Read a encryptedFile from disk.
	/// </summary>
	/// <returns>A a new instance of the <see cref="EncryptedFile"/> class.</returns>
	EncryptedFile readEncryptedFileFromDisk(const char filename[]);
	/// <summary>
	/// Write a encryptedFile to disk.
	/// </summary>
	/// <returns></returns>
	void writeEncryptedFileToDisk(const char filename[], const EncryptedFile& const enc);
}
