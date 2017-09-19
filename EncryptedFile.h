#pragma once
#include <vector>

typedef unsigned char byte;

class EncryptedFile
{

private:
	const std::vector<byte> & data;
	const std::vector<byte> iv;
	const std::vector<byte> salt;
	const std::vector<byte> aad;

public:
	EncryptedFile(std::vector<byte> & data, std::vector<byte> iv, std::vector<byte> salt, std::vector<byte> aad)
		:aad(aad), salt(salt), iv(iv), data(data) {}

	~EncryptedFile();

	std::vector<byte> getData() { return this->data; }
	std::vector<byte> getIV() { return this->iv; }
	std::vector<byte> getSalt() { return this->salt; }
	std::vector<byte> getAAD() { return this->aad; }

};

