#pragma once

#include <stdexcept>

class GeneralSecurityException : public std::runtime_error
{
public:
	GeneralSecurityException() :runtime_error("General security exception has occurred") {}
	explicit GeneralSecurityException(const std::string& whatArg) :runtime_error(whatArg) {}
	~GeneralSecurityException() {}
};
