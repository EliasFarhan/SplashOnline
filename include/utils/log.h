//
// Created by unite on 19.07.2024.
//

#ifndef SPLASHONLINE_LOG_H
#define SPLASHONLINE_LOG_H

#include <string_view>

namespace splash
{

void logDebug(std::string_view msg, std::string_view file, int lineNumber);
void logWarning(std::string_view msg, std::string_view file, int lineNumber);
void logError(std::string_view msg, std::string_view file, int lineNumber);

#define LogError(msg) logError(msg, __FILE__, __LINE__)
#define LogWarning(msg) logWarning(msg, __FILE__, __LINE__)
#define LogDebug(msg) logDebug(msg, __FILE__, __LINE__)

}

#endif //SPLASHONLINE_LOG_H
