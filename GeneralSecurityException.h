#pragma once

#include <stdexcept>

class GeneralSecurityException : public std::runtime_error
{
public:
	GeneralSecurityException() :runtime_error("General security exception has occurred") {}
	GeneralSecurityException(const std::string& what_arg) :runtime_error(what_arg) {}
	~GeneralSecurityException() {}

private:

};
