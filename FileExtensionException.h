#pragma once
#include <iostream>

class FileExtensionException: public std::exception{
public:
	std::string what() {
		return "wrong file extension: extension wasn't .bmp";
	}
};
