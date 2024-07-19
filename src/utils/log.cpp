//
// Created by unite on 19.07.2024.
//

#include "utils/log.h"
#include <iostream>

namespace splash
{

void logError(std::string_view msg, std::string_view file, int lineNumber)
{
	std::cerr << "Error at "<<file<<':'<<lineNumber<<": "<<msg<<'\n';
}

void logWarning(std::string_view msg, std::string_view file, int lineNumber)
{
	std::cerr << "Error at "<<file<<':'<<lineNumber<<": "<<msg<<'\n';
}

void logDebug(std::string_view msg, std::string_view file, int lineNumber)
{
	std::cout << "Error at "<<file<<':'<<lineNumber<<": "<<msg<<'\n';
}
}