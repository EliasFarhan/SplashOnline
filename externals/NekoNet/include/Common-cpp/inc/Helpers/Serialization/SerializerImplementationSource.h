/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Helpers/Serialization/SerializerImplementationProtocol16.h"
#include "Common-cpp/inc/Helpers/Serialization/SerializerImplementationProtocol18.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			template<typename T>
			bool SerializerImplementation::push(const T& data, bool setType)
			{
				switch(mSerializationProtocol)
				{
				case SerializationProtocol::PROTOCOL_1_6:
					return static_cast<SerializerImplementationProtocol16*>(this)->push(data, setType);
				case SerializationProtocol::PROTOCOL_1_8:
					return static_cast<SerializerImplementationProtocol18*>(this)->push(data, setType);
				default:
					EGLOG(DebugLevel::ERRORS, L"failed due to unsupported serialization protocol.");
					return false;
				}
			}

			template<typename T>
			bool SerializerImplementation::push(const T pData, int arraySize, bool setType)
			{
				switch(mSerializationProtocol)
				{
				case SerializationProtocol::PROTOCOL_1_6:
					return static_cast<SerializerImplementationProtocol16*>(this)->push(pData, arraySize, setType);
				case SerializationProtocol::PROTOCOL_1_8:
					return static_cast<SerializerImplementationProtocol18*>(this)->push(pData, arraySize, setType);
				default:
					EGLOG(DebugLevel::ERRORS, L"failed due to unsupported serialization protocol.");
					return false;
				}
			}

			template<typename T>
			bool SerializerImplementation::push(const T pData, const int* arraySizes, bool setType)
			{
				switch(mSerializationProtocol)
				{
				case SerializationProtocol::PROTOCOL_1_6:
					return static_cast<SerializerImplementationProtocol16*>(this)->push(pData, arraySizes, setType);
				case SerializationProtocol::PROTOCOL_1_8:
					return static_cast<SerializerImplementationProtocol18*>(this)->push(pData, arraySizes, setType);
				default:
					EGLOG(DebugLevel::ERRORS, L"failed due to unsupported serialization protocol.");
					return false;
				}
			}
		}
	}
}