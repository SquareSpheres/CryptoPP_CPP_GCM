#pragma once

#include <stdexcept>

class IOException: public std::runtime_error
{
public:
	IOException() :runtime_error("General I/O exception has occurred"){}
	explicit IOException(const std::string& what_arg):runtime_error(what_arg){}
	~IOException(){}
};

