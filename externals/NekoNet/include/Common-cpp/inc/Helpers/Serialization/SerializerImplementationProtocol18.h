/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once
#include <type_traits>
#include "Common-cpp/inc/Helpers/ConfirmAllowed.h"
#include "Common-cpp/inc/Helpers/Serialization/SerializerImplementation.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/CodeForType.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/CodeZero.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/IntegerTypeCode.h"
#include "Common-cpp/inc/Helpers/TypeTraits/IsSame.h"
#include "Common-cpp/inc/Helpers/TypeTraits/MakeUnsigned.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			class SerializerImplementationProtocol18 : public SerializerImplementation
			{
			public:
				SerializerImplementationProtocol18(void);
			private:
				template<typename T> bool push(const T& data, bool setType);
				template<typename T> bool push(const T pData, int arraySize, bool setType);
				template<typename T> bool push(const T pData, const int* arraySizes, bool setType);
				virtual bool push(const Object& pSerObject, bool setType);

				template<typename T> typename EnableIf<IsSame<T, short>::is||IsSame<T, float>::is||IsSame<T, double>::is, bool>::type write(T data, bool setType=false);
				template<typename T> typename EnableIf<IsSame<T, unsigned short>::is, bool>::type write(T data);
				template<typename T> typename EnableIf<IsSame<T, int>::is||IsSame<T, int64>::is, bool>::type write(T data, bool setType=false);
				template<typename T> typename EnableIf<IsSame<T, unsigned int>::is||IsSame<T, uint64>::is, bool>::type write(T data);
				bool write(nByte data, bool setType=false);
				bool write(bool data, bool setType=false);
				bool write(const JString& str, bool setType=false);
				bool write(const Hashtable& hash, bool setType=false);
				bool write(const DictionaryBase& dict, bool setType=false);
				bool write(const CustomTypeBase& data, nByte customType, bool setType=false);
				bool write(const Object& obj, bool setType=false);
				void write(const bool* boolArr, int length, bool setType=false);
				bool writeArray(const Object& obj, bool setType=false);
				bool writeArrayHelper(const void* pData, nByte type, nByte customType, unsigned int dimensions, const int* arraySizes, bool setType, unsigned int recursionDepth);
				bool writeType(const void* pData, nByte type, nByte customType, bool setType, int i=0);
				void writeDictionaryHeader(const DictionaryBase& dict);

				void writeData(const void* data, int size);

				template<typename T> nByte getTypeCode18(void);
				nByte getTypeCode18ForTypeCode(nByte typeCode, unsigned int dimensions=0);

				template<typename T> typename EnableIf<IsSame<T, nByte>::is||IsSame<T, short>::is||IsSame<T, float>::is||IsSame<T, double>::is||IsSame<T, unsigned short>::is, unsigned int>::type getTypeSize(T value=T(), bool setType=false) const;
				template<typename T> typename EnableIf<IsSame<T, int>::is||IsSame<T, int64>::is, unsigned int>::type getTypeSize(T value, bool setType=false) const;
				unsigned int getTypeSize(bool value, bool) const;
				unsigned int getTypeSize(const JString& value, bool setType=false) const;
				unsigned int getTypeSize(const Hashtable& value, bool setType=false) const;
				unsigned int getTypeSize(const DictionaryBase& value, bool setType=false) const;

				template<typename T> typename MakeUnsigned<T>::type encodeZigZag(T value) const;

				unsigned int getCustomSize(const CustomTypeBase& data, nByte customType) const;
				unsigned int getObjectSize(const Object& serObject, bool setType) const;
				unsigned int getByteArraySize(int length) const;
				bool calcArraySize(const void* pData, nByte type, nByte customType, unsigned int dimensions, const int* arraySizes, unsigned int recursionDepth, unsigned int& result) const;

				friend class SerializerImplementation;
			};

			template<typename T>
			bool SerializerImplementationProtocol18::push(const T& data, bool setType)
			{
				extendInternalBuffer(getTypeSize(data, setType));
				return writeType(&data, ConfirmAllowed<T>::typeName, ConfirmAllowed<T>::customTypeName, setType);
			}

			template<typename T>
			bool SerializerImplementationProtocol18::push(const T pData, int arraySize, bool setType)
			{
				unsigned int size = 0;
				calcArraySize(pData, ConfirmAllowed<T>::typeName, ConfirmAllowed<T>::customTypeName, 1, &arraySize, 0, size);
				extendInternalBuffer(size + (setType?getTypeSize<nByte>():0));
				return writeArrayHelper(pData, ConfirmAllowed<T>::typeName, ConfirmAllowed<T>::customTypeName, 1, &arraySize, setType, 0);
			}

			template<typename T>
			bool SerializerImplementationProtocol18::push(const T pData, const int* arraySizes, bool setType)
			{
				unsigned int size = 0;
				calcArraySize(pData, ConfirmAllowed<T>::typeName, ConfirmAllowed<T>::customTypeName, ConfirmAllowed<T>::dimensions, arraySizes, 0, size);
				extendInternalBuffer(size + (setType?getTypeSize<nByte>():0));
				return writeArrayHelper(pData, ConfirmAllowed<T>::typeName, ConfirmAllowed<T>::customTypeName, ConfirmAllowed<T>::dimensions, arraySizes, setType, 0);
			}

			template<typename T>
			typename EnableIf<IsSame<T, short>::is||IsSame<T, float>::is||IsSame<T, double>::is, bool>::type SerializerImplementationProtocol18::write(T data, bool setType)
			{
				if(setType)
				{
					if(!data)
						return write(TypeCode18::CodeZero<T>::typeCode);
					else
						write(TypeCode18::CodeForType<T>::typeCode);
				}
				writeData(&data, getTypeSize<T>());
				return true;
			}

			template<typename T>
			typename EnableIf<IsSame<T, unsigned short>::is, bool>::type SerializerImplementationProtocol18::write(T data)
			{
				writeData(&data, getTypeSize<T>());
				return true;
			}

			template<typename T>
			typename EnableIf<IsSame<T, int>::is||IsSame<T, int64>::is, bool>::type SerializerImplementationProtocol18::write(T data, bool setType)
			{
				if(setType)
				{
					if(!data)
						return write(TypeCode18::IntegerTypeCode<T>::ZERO);
					else if(data>0)
					{
						if(data<=static_cast<int>(UCHAR_MAX))
						{
							write(TypeCode18::IntegerTypeCode<T>::ONE_BYTE);
							return write(static_cast<nByte>(data));
						}
						else if(data<=static_cast<int>(USHRT_MAX))
						{
							write(TypeCode18::IntegerTypeCode<T>::TWO_BYTE);
							return write(static_cast<unsigned short>(data));
						}
					}
					else if(data>=-static_cast<int>(UCHAR_MAX))
					{
						write(TypeCode18::IntegerTypeCode<T>::ONE_BYTE_);
						return write(static_cast<nByte>(-data));
					}
					else if(data>=-static_cast<int>(USHRT_MAX))
					{
						write(TypeCode18::IntegerTypeCode<T>::TWO_BYTE_);
						return write(static_cast<unsigned short>(-data));
					}
					write(TypeCode18::IntegerTypeCode<T>::COMPRESSED);
				}
				uint64 zigVal = encodeZigZag(data);
				write(zigVal);
				return true;
			}

			template<typename T>
			typename EnableIf<IsSame<T, unsigned int>::is||IsSame<T, uint64>::is, bool>::type SerializerImplementationProtocol18::write(T data)
			{
				int count = 0;
				nByte buffer[sizeof(T)*CHAR_BIT/(CHAR_BIT-1)+(sizeof(T)*CHAR_BIT%(CHAR_BIT-1)?1:0)];
				for(; buffer[count++] = static_cast<nByte>(data & 0x7F), (data=data>>(CHAR_BIT-1)) != 0; buffer[count-1] |= 0x80);
				writeData(buffer, count);
				return true;
			}

			template<typename T>
			nByte SerializerImplementationProtocol18::getTypeCode18(void)
			{
				return TypeCode18::CodeForType<T>::typeCode;
			}

			template<typename T>
			typename EnableIf<IsSame<T, nByte>::is||IsSame<T, short>::is||IsSame<T, float>::is||IsSame<T, double>::is||IsSame<T, unsigned short>::is, unsigned int>::type SerializerImplementationProtocol18::getTypeSize(T value, bool setType) const
			{
				return (setType?getTypeSize<nByte>():0) + (value||!setType?sizeof(T):0);
			}

			template<typename T>
			typename EnableIf<IsSame<T, int>::is||IsSame<T, int64>::is, unsigned int>::type SerializerImplementationProtocol18::getTypeSize(T value, bool setType) const
			{
				if(setType)
				{
					if(!value)
						return getTypeSize<nByte>();
					if(value>=-static_cast<int>(UCHAR_MAX)&&value<=static_cast<int>(UCHAR_MAX))
						return getTypeSize<nByte>()+1;
					if(value>=-static_cast<int>(USHRT_MAX)&&value<=static_cast<int>(USHRT_MAX))
						return getTypeSize<nByte>()+2;
				}

				uint64 zigVal = encodeZigZag(value);
				int size = 1;
				for(; (zigVal=zigVal>>(CHAR_BIT-1)) != 0; ++size);
				return (setType?getTypeSize<nByte>():0) + size;
			}

			template<typename T>
			typename MakeUnsigned<T>::type SerializerImplementationProtocol18::encodeZigZag(T value) const
			{
				return static_cast<typename MakeUnsigned<T>::type>((value << 1) ^ (value >> (sizeof(T)*CHAR_BIT-1)));
			}
		}
	}
}