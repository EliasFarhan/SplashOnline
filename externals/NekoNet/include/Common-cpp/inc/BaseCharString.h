/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Base.h"

namespace ExitGames
{
	namespace Common
	{
		class JString;

		class BaseCharString : public Base
		{
		public:
			using ToString::toString;
			virtual ~BaseCharString(void);
			virtual operator const char* (void) const = 0;
			virtual operator JString (void) const = 0;

			const char* cstr(void) const;
			virtual JString JStringRepresentation(void) const = 0;
			unsigned int length(void) const;
			virtual unsigned int size(void) const = 0;
			virtual JString& toString(JString& retStr, bool withTypes=false) const;
		protected:
			BaseCharString(unsigned int length=0);

			char* mBuffer;
			unsigned int mLength;
		};
	}
}