/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Containers/DictionaryBase.h"
#include "Common-cpp/inc/Helpers/Serialization/DeserializerImplementation.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/CodeForType.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/Is1DArray.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/IsCustom.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/IsValid.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/ObjectTypeCodeForCode.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode18/UnsignedCode.h"
#include "Common-cpp/inc/Helpers/TypeTraits/MakeSigned.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			class DeserializerImplementationProtocol18 : public DeserializerImplementation
			{
			public:
				using ToString::toString;

				DeserializerImplementationProtocol18(const nByte* data, unsigned int size);

				virtual bool pop(Object& object);
				virtual bool popByte(Object& object);
				template<nByte C> bool pop(typename EnableIf<!TypeCode18::IsValid<C>::is, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<TypeCode18::IsValid<C>::is&&C!=TypeCode18::EG_NULL&&!TypeCode18::IsArray<C>::is&&C!=TypeCode18::STRING&&C!=TypeCode18::DICTIONARY&&!TypeCode18::IsCustom<C>::is&&C!=TypeCode18::UNKNOWN, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode18::EG_NULL, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<TypeCode18::Is1DArray<C>::is&&C!=TypeCode18::BOOLEAN_ARRAY&&C!=TypeCode18::STRING_ARRAY&&C!=TypeCode18::DICTIONARY_ARRAY&&C!=TypeCode18::CUSTOM_TYPE_ARRAY, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode18::ARRAY, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode18::BOOLEAN_ARRAY, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode18::STRING, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode18::STRING_ARRAY, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode18::DICTIONARY, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode18::DICTIONARY_ARRAY, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<TypeCode18::IsCustom<C>::is, Object>::type& object, nByte customTypeCode=0);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode18::CUSTOM_TYPE_ARRAY, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode18::UNKNOWN, Object>::type& object);

				virtual nByte readByte(void);
				virtual short readShort(void);
				template<nByte C> typename TypeCode18::TypeForCode<C>::type read(typename EnableIf<IsArithmetic<typename TypeCode18::TypeForCode<C>::type>::is>::type* = NULL);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::BYTE||C==TypeCode18::BOOLEAN, typename TypeCode18::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::SHORT||C==TypeCode18::FLOAT||C==TypeCode18::DOUBLE, typename TypeCode18::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::INT1||C==TypeCode18::L1, typename TypeCode18::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::INT1_||C==TypeCode18::L1_, typename TypeCode18::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::INT2||C==TypeCode18::L2, typename TypeCode18::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::INT2_||C==TypeCode18::L2_, typename TypeCode18::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::COMPRESSED_INT||C==TypeCode18::COMPRESSED_LONG, typename TypeCode18::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::COMPRESSED_UINT||C==TypeCode18::COMPRESSED_ULONG, typename TypeCode18::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::BOOLEAN_FALSE, bool>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::BOOLEAN_TRUE, bool>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::BYTE_ZERO||C==TypeCode18::SHORT_ZERO||C==TypeCode18::INT_ZERO||C==TypeCode18::LONG_ZERO||C==TypeCode18::FLOAT_ZERO||C==TypeCode18::DOUBLE_ZERO, typename TypeCode18::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::STRING, JString>::type& retVal, int size);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::HASHTABLE, Hashtable>::type& retVal);
				template<nByte C> void read(typename EnableIf<TypeCode18::IsCustom<C>::is, CustomTypeBase>::type& retVal, nByte customTypeCode);
				template<nByte C> void read(typename EnableIf<C==TypeCode18::UNKNOWN, Object>::type& retVal);
			private:
				template<typename T> typename MakeSigned<T>::type decodeZigZag(T value) const;
				template<nByte C> nByte readCustomTypeCode(typename EnableIf<C==TypeCode18::CUSTOM>::type* = NULL);
				template<nByte C> nByte readCustomTypeCode(typename EnableIf<C!=TypeCode18::CUSTOM>::type* = NULL) const;
				nByte getObjectTypeCodeForCode(nByte code);
				bool popType(nByte typeCode, Object& object);
				void popArrayOfDictionaries(Object& dict, unsigned int dimensions=0);
				const DictionaryBase* popDictionaryBody(nByte* pKeyTypes, nByte* pValueTypes, unsigned int* pDimensionsOfNestedValues, nByte customType);
				void readDictionaryTypes(nByte** ppKeyTypes, nByte** ppValTypes, unsigned int** ppDimensionsOfNestedValues, nByte* pCustomType);
			};



			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<!TypeCode18::IsValid<C>::is, Object>::type& /*object*/)
			{
				EGLOG(DebugLevel::ERRORS, L"unsupported type code %hd", C);
				return false;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<TypeCode18::IsValid<C>::is&&C!=TypeCode18::EG_NULL&&!TypeCode18::IsArray<C>::is&&C!=TypeCode18::STRING&&C!=TypeCode18::DICTIONARY&&!TypeCode18::IsCustom<C>::is&&C!=TypeCode18::UNKNOWN, Object>::type& object)
			{
				typedef typename TypeCode18::TypeForCode<C>::type T;
				typedef typename Conditional<IsArithmetic<T>::is, T, T& >::type TR;
				TR data = alloc<T>();
				read<C>(data);
				object.set(&data, TypeCode18::ObjectTypeCodeForCode<C>::code, 0, IsArithmetic<T>::is);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<C==TypeCode18::EG_NULL, Object>::type& object)
			{
				object.setToNULL();
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<TypeCode18::Is1DArray<C>::is&&C!=TypeCode18::BOOLEAN_ARRAY&&C!=TypeCode18::STRING_ARRAY&&C!=TypeCode18::DICTIONARY_ARRAY&&C!=TypeCode18::CUSTOM_TYPE_ARRAY, Object>::type& object)
			{
				typedef typename TypeCode18::TypeForCode<C>::type T;
				unsigned int size = read<TypeCode18::COMPRESSED_UINT>();
				T* arr = MemoryManagement::allocateArray<T>(size);
				for(unsigned int i=0; i<size; ++i)
					read<TypeCode18::CodeForType<T>::typeCode>(arr[i]); // 'CodeForType<T>::typeCode' instead of just 'C' to get the non-array typecode instead of the array typecode
				object.set(arr, TypeCode18::ObjectTypeCodeForCode<C>::code, 0, size, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<C==TypeCode18::ARRAY, Object>::type& object)
			{
				typedef typename TypeCode18::TypeForCode<C>::type T;
				unsigned int size = read<TypeCode18::COMPRESSED_UINT>();
				Object innerArray;
				int* sizes;
				T* arr = MemoryManagement::allocateArray<T>(size);
				for(unsigned int i=0; i<size; ++i)
				{
					pop(innerArray);
					arr[i] = const_cast<T>(innerArray.getData());
					if(i == size-1)
						break; // wait with deleting the last of the innerArray's, until we have have extracted type, dimensions and sizes (they will be identical for all inner arrays)
					innerArray.setDataNoCopy(NULL); // prevent the destructor of innerArray at the end of it's scope from freeing the payload, so we can just copy the address above, as recursive deep-copies of arrays can be very expensive
					innerArray.setType(TypeCode::EG_NULL); // part 2 of payload prevention
				}

				sizes = MemoryManagement::allocateArray<int>(innerArray.getDimensions()+1);
				sizes[0] = size;
				for(unsigned int j=1; j<(innerArray.getDimensions()+1); ++j)
					sizes[j] = innerArray.getSizes()[j-1];
				object.set(arr, innerArray.getType(), innerArray.getCustomType(), innerArray.getDimensions()+1, sizes, false);
				innerArray.setDataNoCopy(NULL);
				innerArray.setType(TypeCode::EG_NULL);
				MemoryManagement::deallocateArray(sizes);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<C==TypeCode18::BOOLEAN_ARRAY, Object>::type& object)
			{
				unsigned int size = read<TypeCode18::COMPRESSED_UINT>();
				bool* arr = MemoryManagement::allocateArray<bool>(size);
				int fullByteCount = size / CHAR_BIT;
				unsigned int i = 0;

				while(fullByteCount)
				{
					nByte temp = read<TypeCode18::BYTE>();
					arr[i++] = !!(temp & 0x01);
					arr[i++] = !!(temp & 0x02);
					arr[i++] = !!(temp & 0x04);
					arr[i++] = !!(temp & 0x08);
					arr[i++] = !!(temp & 0x10);
					arr[i++] = !!(temp & 0x20);
					arr[i++] = !!(temp & 0x40);
					arr[i++] = !!(temp & 0x80);

					--fullByteCount;
				}

				if(i < size)
				{
					nByte temp = read<TypeCode18::BYTE>();
					int shift = 0;
					while(i < size)
					{
						arr[i++] = !!(temp & 1<<shift);
						++shift;
					}
				}

				object.set(arr, TypeCode::BOOLEAN, 0, size, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<C==TypeCode18::STRING, Object>::type& object)
			{
				int size = read<TypeCode18::COMPRESSED_UINT>();
				JString& data = *MemoryManagement::allocateArray<JString>(1, size);
				read<C>(data, size);
				object.set(&data, TypeCode18::ObjectTypeCodeForCode<C>::code, 0, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<C==TypeCode18::STRING_ARRAY, Object>::type& object)
			{
				unsigned int size = read<TypeCode18::COMPRESSED_UINT>();
				int sizeOfBiggestString = 0;
				int oldDataOffset = mDataOffset;
				for(unsigned int i=0; i<size; ++i)
				{
					int strSize = read<TypeCode18::COMPRESSED_UINT>();
					if(strSize>sizeOfBiggestString)
						sizeOfBiggestString = strSize;
					mDataOffset += strSize;
				}
				mDataOffset = oldDataOffset;
				JString* arr = MemoryManagement::allocateArray<JString>(size, sizeOfBiggestString);
				for(unsigned int i=0; i<size; ++i)
				{
					int strSize = read<TypeCode18::COMPRESSED_UINT>();
					arr[i].setFromUTF8Characters(reinterpret_cast<const char*>(mpData+mDataOffset), strSize);
					mDataOffset += strSize;
				}
				object.set(arr, TypeCode18::ObjectTypeCodeForCode<C>::code, 0, size, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<C==TypeCode18::DICTIONARY, Object>::type& object)
			{
				popArrayOfDictionaries(object);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<C==TypeCode18::DICTIONARY_ARRAY, Object>::type& object)
			{
				popArrayOfDictionaries(object, 1);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<TypeCode18::IsCustom<C>::is, Object>::type& object, nByte customTypeCode)
			{
				if(!customTypeCode)
					customTypeCode = readCustomTypeCode<C>();
				CustomTypeBase& data = *CustomTypeBase::allocObject(1, customTypeCode);
				read<C>(data, customTypeCode);
				object.set(&data, TypeCode::CUSTOM, customTypeCode, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<C==TypeCode18::CUSTOM_TYPE_ARRAY, Object>::type& object)
			{
				unsigned int size = read<TypeCode18::COMPRESSED_UINT>();
				nByte customTypeCode = read<TypeCode18::BYTE>();
				CustomTypeBase* arr = CustomTypeBase::allocObject(size, customTypeCode);
				for(unsigned int i=0; i<size; ++i)
					read<TypeCode18::CUSTOM>(*reinterpret_cast<CustomTypeBase*>(reinterpret_cast<nByte*>(arr)+CustomTypeBase::getSizeof(customTypeCode)*i), customTypeCode);
				object.set(arr, TypeCode::CUSTOM, customTypeCode, size, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol18::pop(typename EnableIf<C==TypeCode18::UNKNOWN, Object>::type& object)
			{
				return popType(read<TypeCode18::BYTE>(), object);
			}

			template<nByte C>
			typename TypeCode18::TypeForCode<C>::type DeserializerImplementationProtocol18::read(typename EnableIf<IsArithmetic<typename TypeCode18::TypeForCode<C>::type>::is>::type*)
			{
				typename TypeCode18::TypeForCode<C>::type retVal;
				read<C>(retVal);
				return retVal;
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::BYTE||C==TypeCode18::BOOLEAN, typename TypeCode18::TypeForCode<C>::type>::type& retVal)
			{
				retVal = *reinterpret_cast<const typename TypeCode18::TypeForCode<C>::type*>(mpData+(mDataOffset++));
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::SHORT||C==TypeCode18::FLOAT||C==TypeCode18::DOUBLE, typename TypeCode18::TypeForCode<C>::type>::type& retVal)
			{
				typedef typename TypeCode18::TypeForCode<C>::type T;
#ifdef _EG_BIGENDIAN_PLATFORM
				for(int i=sizeof(T)-1; i>=0; --i)
					reinterpret_cast<nByte*>(&retVal)[i] = read<TypeCode::BYTE>();
#else
				MEMCPY(&retVal, mpData+mDataOffset, sizeof(T));
				mDataOffset += sizeof(T);
#endif
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::INT1||C==TypeCode18::L1, typename TypeCode18::TypeForCode<C>::type>::type& retVal)
			{
				retVal = read<TypeCode18::BYTE>();
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::INT1_||C==TypeCode18::L1_, typename TypeCode18::TypeForCode<C>::type>::type& retVal)
			{
				retVal = -read<TypeCode18::BYTE>();
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::INT2||C==TypeCode18::L2, typename TypeCode18::TypeForCode<C>::type>::type& retVal)
			{
				retVal = static_cast<unsigned short>(read<TypeCode18::SHORT>());
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::INT2_||C==TypeCode18::L2_, typename TypeCode18::TypeForCode<C>::type>::type& retVal)
			{
				retVal = -static_cast<unsigned short>(read<TypeCode18::SHORT>());
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::COMPRESSED_INT||C==TypeCode18::COMPRESSED_LONG, typename TypeCode18::TypeForCode<C>::type>::type& retVal)
			{
				retVal = decodeZigZag(read<TypeCode18::UnsignedCode<C>::typeCode>());
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::COMPRESSED_UINT||C==TypeCode18::COMPRESSED_ULONG, typename TypeCode18::TypeForCode<C>::type>::type& retVal)
			{
				typedef typename TypeCode18::TypeForCode<C>::type T;
				T value = 0;
				unsigned int shift = 0;

				while(shift < (sizeof(T)*CHAR_BIT/(CHAR_BIT-1)+(sizeof(T)*CHAR_BIT%(CHAR_BIT-1)?1:0))*(CHAR_BIT-1))
				{
					nByte b = mpData[mDataOffset];
					++mDataOffset;

					value |= static_cast<T>(b & 0x7F) << shift;
					shift += CHAR_BIT-1;
					if((b & 0x80) == 0)
						break;
				}
				retVal = value;
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::BOOLEAN_FALSE, bool>::type& retVal)
			{
				retVal = false;
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::BOOLEAN_TRUE, bool>::type& retVal)
			{
				retVal = true;
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::BYTE_ZERO||C==TypeCode18::SHORT_ZERO||C==TypeCode18::INT_ZERO||C==TypeCode18::LONG_ZERO||C==TypeCode18::FLOAT_ZERO||C==TypeCode18::DOUBLE_ZERO, typename TypeCode18::TypeForCode<C>::type>::type& retVal)
			{
				retVal = 0;
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::STRING, JString>::type& retVal, int size)
			{
				retVal.setFromUTF8Characters(reinterpret_cast<const char*>(mpData+mDataOffset), size);
				mDataOffset += size;
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::HASHTABLE, Hashtable>::type& retVal)
			{
				int size = read<TypeCode18::COMPRESSED_UINT>();
				for(int i=0; i<size; ++i)
				{
					Object k, v;
					pop(k);
					pop(v);
					retVal.template put<Object, Object>(k, v);
				}
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<TypeCode18::IsCustom<C>::is, CustomTypeBase>::type& retVal, nByte customTypeCode)
			{
				short length = static_cast<short>(read<TypeCode18::COMPRESSED_UINT>());
				CustomTypeBase::deserialize(mpData+mDataOffset, length, reinterpret_cast<void*>(&retVal), customTypeCode);
				mDataOffset += length;
			}

			template<nByte C>
			void DeserializerImplementationProtocol18::read(typename EnableIf<C==TypeCode18::UNKNOWN, Object>::type& retVal)
			{
				pop(retVal);
			}

			template<typename T>
			typename MakeSigned<T>::type DeserializerImplementationProtocol18::decodeZigZag(T value) const
			{
				return static_cast<typename MakeSigned<T>::type>((value >> 1) ^ -static_cast<typename MakeSigned<T>::type>(value & 1));
			}

			template<nByte C>
			nByte DeserializerImplementationProtocol18::readCustomTypeCode(typename EnableIf<C==TypeCode18::CUSTOM>::type*)
			{
				return read<TypeCode18::BYTE>();
			}

			template<nByte C>
			nByte DeserializerImplementationProtocol18::readCustomTypeCode(typename EnableIf<C!=TypeCode18::CUSTOM>::type*) const
			{
				return C-TypeCode18::CUSTOM_TYPE_SLIM;
			}
		}
    }
}
