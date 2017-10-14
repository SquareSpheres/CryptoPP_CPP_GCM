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
	EncryptedFile(std::vector<byte> &data, std::vector<byte> iv, std::vector<byte> salt, std::vector<byte> aad)
		:data(data), iv(iv), salt(salt), aad(aad)
	{
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
	const std::vector<byte> * getData() const { return &this->data; }
	/// <summary>
	/// Gets the iv.
	/// </summary>
	/// <returns></returns>
	const std::vector<byte> * getIV() const { return &this->iv; }
	/// <summary>
	/// Gets the salt.
	/// </summary>
	/// <returns></returns>
	const std::vector<byte> * getSalt() const { return &this->salt; }
	/// <summary>
	/// Gets the additional authenticated data.
	/// </summary>
	/// <returns></returns>
	const std::vector<byte> * getAAD() const { return &this->aad; }


	/// <summary>
	/// Read a encryptedFile from disk.
	/// </summary>
	/// <param name="filename">File location</param>
	/// <returns>
	/// A a new instance of the <see cref="EncryptedFile" /> class.
	/// </returns>
	static EncryptedFile readEncryptedFileFromDisk(const std::string &filename);
	/// <summary>
	/// Write a encryptedFile to disk.
	/// </summary>
	/// <param name="filename">File location</param>
	/// <param name="enc">The EncryptedFile object</param>
	static void writeEncryptedFileToDisk(const std::string &filename, EncryptedFile&  enc);
	/// <summary>
	/// Determines whether a file is a serialized EncryptedFile.
	/// </summary>
	/// <param name="filename">File location</param>
	/// <returns>
	///   <c>true</c> if [is of type EncryptedFile]; otherwise, <c>false</c>.
	/// </returns>
	static bool isEncryptedFile(const std::string &filename);

};


