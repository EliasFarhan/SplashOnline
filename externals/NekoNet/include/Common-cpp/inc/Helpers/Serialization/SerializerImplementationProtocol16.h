/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Helpers/ConfirmAllowed.h"
#include "Common-cpp/inc/Helpers/Serialization/SerializerImplementation.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			class SerializerImplementationProtocol16 : public SerializerImplementation
			{
			public:
				SerializerImplementationProtocol16(void);
			private:
				template<typename T> bool push(const T& data, bool setType);
				template<typename T> bool push(const T pData, int arraySize, bool setType);
				template<typename T> bool push(const T pData, const int* arraySizes, bool setType);
				virtual bool push(const Object& pSerObject, bool setType);

				template<typename T> bool write(T data, bool ignored=false);
				template<typename K, typename V> bool write(const Dictionary<K, V>& data, bool setKeyValueTypes=false);
				template<typename T, nByte typeCode> bool write(const CustomType<T, typeCode>& data, bool=false);
				bool write(nByte data, bool=false);
				bool write(const JString& str, bool=false);
				bool write(const Hashtable& hash, bool=false);
				bool write(const DictionaryBase& dict, bool setKeyValueTypes);
				bool write(const CustomTypeBase& data, nByte customType);
				bool write(const Object& obj, bool setType);
				void write(const nByte* byteArr, int length);
				bool writeArray(const Object& obj, bool setType);
				bool writeArrayHelper(const void* pData, nByte type, nByte customType, unsigned int dimensions, const int* arraySizes, unsigned int recursionDepth);
				bool writeArrayType(const void* pData, nByte type, nByte customType, short i);

				void writeData(const void* data, int size);

				template<typename T> unsigned int getTypeSize(T=T()) const;
				unsigned int getTypeSize(const JString& value) const;
				unsigned int getTypeSize(const Hashtable& value) const;
				unsigned int getTypeSize(const DictionaryBase& value) const;

				unsigned int getCustomSize(const CustomTypeBase& data, nByte customType) const;
				unsigned int getObjectSize(const Object& serObject, bool setType) const;
				unsigned int getByteArraySize(int length) const;
				bool calcArraySize(const void* pData, nByte type, nByte customType, unsigned int dimensions, const int* arraySizes, unsigned int recursionDepth, unsigned int& result) const;

				friend class SerializerImplementation;
			};



			template<typename T>
			bool SerializerImplementationProtocol16::push(const T& data, bool setType)
			{
				extendInternalBuffer(getTypeSize(data) + (setType?getTypeSize<nByte>():0));
				if(setType)
					write(ConfirmAllowed<T>::typeName);
				return write(data, setType);
			}

			template<typename T>
			bool SerializerImplementationProtocol16::push(const T pData, int arraySize, bool setType)
			{
				unsigned int size = 0;
				calcArraySize(pData, ConfirmAllowed<T>::typeName, ConfirmAllowed<T>::customTypeName, 1, &arraySize, 0, size);
				extendInternalBuffer(size + (setType?getTypeSize<nByte>():0));
				if(setType)
					write(ConfirmAllowed<T>::typeName==TypeCode::OBJECT?TypeCode::OBJECT:TypeCode::ARRAY);
				return writeArrayHelper(pData, ConfirmAllowed<T>::typeName, ConfirmAllowed<T>::customTypeName, 1, &arraySize, 0);
			}

			template<typename T>
			bool SerializerImplementationProtocol16::push(const T pData, const int* arraySizes, bool setType)
			{
				unsigned int size = 0;
				calcArraySize(pData, ConfirmAllowed<T>::typeName, ConfirmAllowed<T>::customTypeName, ConfirmAllowed<T>::dimensions, arraySizes, 0, size);
				extendInternalBuffer(size + (setType?getTypeSize<nByte>():0));
				if(setType)
					write(ConfirmAllowed<T>::typeName);
				return writeArrayHelper(pData, ConfirmAllowed<T>::typeName, ConfirmAllowed<T>::customTypeName, ConfirmAllowed<T>::dimensions, arraySizes, 0);
			}

			template<typename T>
			bool SerializerImplementationProtocol16::write(T data, bool /*ignored*/)
			{
				writeData(&data, getTypeSize<T>());
				return true;
			}

			template<typename K, typename V>
			bool SerializerImplementationProtocol16::write(const Dictionary<K, V>& data, bool setKeyValueTypes)
			{
				return write(static_cast<const DictionaryBase&>(data), setKeyValueTypes);
			}

			template<typename T, nByte typeCode>
			bool SerializerImplementationProtocol16::write(const CustomType<T, typeCode>& data, bool /*ignored*/)
			{
				return write(static_cast<const CustomTypeBase&>(data), typeCode);
			}

			template<typename T>
			unsigned int SerializerImplementationProtocol16::getTypeSize(T) const
			{
				return sizeof(T);
			}
		}
	}
}