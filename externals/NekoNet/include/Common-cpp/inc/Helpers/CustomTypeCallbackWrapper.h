/* Exit Games Common - C++ Client Lib
* Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
* https://www.photonengine.com
* mailto:developer@photonengine.com
*/

#pragma once

#include "Common-cpp/inc/defines.h"

typedef void* (*CB_CALLOC)(int count, nByte customTypeCode);
typedef void (*CB_FREE)(const void* pData, nByte customTypeCode);
typedef unsigned int (*CB_SIZEOF)(nByte customTypeCode);

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			class CustomTypeCallbackWrapper
			{
			public:
				CustomTypeCallbackWrapper(void);
				CustomTypeCallbackWrapper(CB_CALLOC cbCalloc, CB_FREE cbFree, CB_SIZEOF cbSizeOf);
				virtual ~CustomTypeCallbackWrapper(void);

				virtual bool compare(const void* pData1, const void* pData2);
				virtual void duplicate(const void* pData, void* retVal);
				virtual void deserialize(const nByte* pData, short length, void* retVal);
				virtual short serialize(const void* pData, nByte* retVal);
				virtual unsigned int toString(const void* pData, EG_CHAR* buffer);
				virtual CustomTypeCallbackWrapper* copy(void) const;
				void* alloc(int count, nByte customTypeCode);
				void free(const void* pData, nByte customTypeCode);
				unsigned int getSizeof(nByte customTypeCode);
				void reset(void);

			protected:
				CB_CALLOC mcbCalloc;
				CB_FREE mcbFree;
				CB_SIZEOF mcbSizeOf;
			};
		}
	}
}