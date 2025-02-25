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
		class ASCIIString : public BaseCharString
		{
		public:
			ASCIIString(void);
			ASCIIString(const ASCIIString& str);
			ASCIIString(const JString& str);
			ASCIIString(const char* str);
			ASCIIString(const EG_CHAR* str);
			~ASCIIString(void);

			ASCIIString& operator=(const ASCIIString& Rhs);
			ASCIIString& operator=(const JString& Rhs);
			ASCIIString& operator=(const char* Rhs);
			ASCIIString& operator=(const EG_CHAR* Rhs);

			operator const char* (void) const;
			operator JString (void) const;

			JString JStringRepresentation(void) const;
			unsigned int size(void) const;
		};
	}
}