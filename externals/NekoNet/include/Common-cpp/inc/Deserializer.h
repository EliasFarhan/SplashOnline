/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Enums/SerializationProtocol.h"
#include "Common-cpp/inc/Object.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			class DeserializerImplementation;
		}

		class Deserializer : public Base
		{
		public:
			using ToString::toString;

			Deserializer(const nByte* data, unsigned int size, nByte protocol=SerializationProtocol::DEFAULT);
			virtual ~Deserializer(void);

			bool pop(Object& object);

			virtual JString& toString(JString& retStr, bool withTypes=false) const;
		private:
			Helpers::DeserializerImplementation& mImp;
		};
	}
}
