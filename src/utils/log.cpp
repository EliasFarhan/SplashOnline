//
// Created by unite on 19.07.2024.
//

#include "utils/log.h"
#include <SDL_log.h>

namespace splash
{

void logError(std::string_view msg, std::string_view file, int lineNumber)
{
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s at %s:%d", msg.data(), file.data(), lineNumber);
}

void logWarning(std::string_view msg, std::string_view file, int lineNumber)
{
	SDL_LogWarn(SDL_LOG_CATEGORY_ERROR,"%s at %s:%d", msg.data(), file.data(), lineNumber);
}

void logDebug(std::string_view msg, std::string_view file, int lineNumber)
{
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s at %s:%d", msg.data(), file.data(), lineNumber);
}
}