/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Base.h"
#include "Common-cpp/inc/Enums/SerializationProtocol.h"
#ifdef EG_PLATFORM_SUPPORTS_MOVE_SEMANTICS
#	include "Common-cpp/inc/Helpers/Data.h"
#endif

namespace ExitGames
{
	namespace Common
	{
		class DictionaryBase;
		class Hashtable;
		class JString;
		class Object;

		namespace Helpers
		{
			class SerializerImplementation : public Base
			{
			public:
				using ToString::toString;

				SerializerImplementation(void);
				virtual ~SerializerImplementation(void);

				const nByte* getData(void) const;
				int getDataOffset(void) const;

				template<typename T> bool push(const T& data, bool setType);
				template<typename T> bool push(const T pData, int arraySize, bool setType);
				template<typename T> bool push(const T pData, const int* arraySizes, bool setType);
				virtual bool push(const Object& pSerObject, bool setType) = 0;

				virtual JString& toString(JString& retStr, bool withTypes=false) const;

#ifdef EG_PLATFORM_SUPPORTS_MOVE_SEMANTICS
				Helpers::Data release(void);
#endif
			protected:
				void extendInternalBuffer(int size);

				int mBufferSize;
				int mDataOffset;
				nByte* mpData;
				nByte mSerializationProtocol;
			};
		}
	}
}