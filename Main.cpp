
#include "FileEncrypter.h"
#include "Utils.h"
#include "EncryptedFile.h"


void printVector(std::vector<byte> &vec) {
	for (std::vector<byte>::const_iterator i = vec.begin(); i != vec.end(); ++i)
		std::cout << *i << ' ';
	std::cout << std::endl;
}

int main(int argc, char* argv[])
{
	FileEncrypter enc;
	std::string encryptedData = enc.cipherData("kalle", (byte*)"kalle", 6);
	std::cout << "encrypted data = " << encryptedData << std::endl;

	std::vector<char> fileData = fileUtils::ReadAllBytes("Does not Exist!");
	std::cout << fileData.size() << std::endl;
	
	std::vector<byte> data = { 'a','b','c' };
	std::vector<byte> salt = { 'a','b','c' };
	std::vector<byte> iv = { 'a','b','c' };
	std::vector<byte> aad = { 'a','b','c' };

	printVector(data);
	printVector(salt);
	printVector(iv);
	printVector(aad);

	EncryptedFile file(data, iv, salt, aad);
	
	// change data
	data[0] = 'A';
	salt[0] = 'A';
	iv[0] = 'A';
	aad[0] = 'A';


	printVector(file.getData());
	printVector(file.getSalt());
	printVector(file.getIV());
	printVector(file.getAAD());




	int wait;
	std::cin >> wait;
}
