/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/BaseCharString.h"

namespace ExitGames
{
	namespace Common
	{
		class UTF8String : public BaseCharString
		{
		public:
			UTF8String(void);
			UTF8String(const UTF8String& str);
			UTF8String(const JString& str);
			UTF8String(const char* str);
			UTF8String(const EG_CHAR* str);
			~UTF8String(void);

			UTF8String& operator=(const UTF8String& Rhs);
			UTF8String& operator=(const JString& Rhs);
			UTF8String& operator=(const char* Rhs);
			UTF8String& operator=(const EG_CHAR* Rhs);

			operator const char* (void) const;
			operator JString (void) const;

			JString JStringRepresentation(void) const;
			unsigned int size(void) const;

			static unsigned int size(const JString& str);
			static unsigned int countOfWideCharsForSize(const char* str, unsigned int size);
		private:
			static unsigned int bytesPerChar(char c);
			static unsigned int lenForSize(const char* str, unsigned int size);
			static unsigned int sizeForLen(const char* str, unsigned int len);

			unsigned int mSize;
		};
	}
}