/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/ASCIIString.h"
#include "Common-cpp/inc/Containers/DictionaryBase.h"
#include "Common-cpp/inc/Helpers/ArrayLengthType.h"
#include "Common-cpp/inc/Helpers/Serialization/DeserializerImplementation.h"
#include "Common-cpp/inc/Helpers/Serialization/TypeCode/IsValid1DArrayElement.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			class DeserializerImplementationProtocol16 : public DeserializerImplementation
			{
			public:
				using ToString::toString;

				DeserializerImplementationProtocol16(const nByte* data, int size);

				virtual bool pop(Object& object);
				virtual bool popByte(Object& object);
				template<nByte C> bool pop(typename EnableIf<!TypeCode::IsValid<C>::is, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<TypeCode::IsValid<C>::is&&C!=TypeCode::EG_NULL&&C!=TypeCode::BYTEARRAY&&C!=TypeCode::OBJECT&&C!=TypeCode::ARRAY&&C!=TypeCode::STRING&&C!=TypeCode::DICTIONARY&&C!=TypeCode::CUSTOM&&C!=TypeCode::UNKNOWN, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode::EG_NULL, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode::BYTEARRAY, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode::OBJECT, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode::ARRAY, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode::STRING, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode::DICTIONARY, Object>::type& object);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode::CUSTOM, Object>::type& object, nByte customTypeCode=0);
				template<nByte C> bool pop(typename EnableIf<C==TypeCode::UNKNOWN, Object>::type& object);
				
				virtual nByte readByte(void);
				virtual short readShort(void);
				template<nByte C> typename TypeCode::TypeForCode<C>::type read(typename EnableIf<IsArithmetic<typename TypeCode::TypeForCode<C>::type>::is>::type* = NULL);
				template<nByte C> void read(typename EnableIf<C==TypeCode::BYTE||C==TypeCode::BOOLEAN, typename TypeCode::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode::SHORT||C==TypeCode::INTEGER||C==TypeCode::LONG||C==TypeCode::FLOAT||C==TypeCode::DOUBLE, typename TypeCode::TypeForCode<C>::type>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode::STRING, JString>::type& retVal, short size);
				template<nByte C> void read(typename EnableIf<C==TypeCode::HASHTABLE, Hashtable>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode::CUSTOM, CustomTypeBase>::type& retVal, nByte customTypeCode);
				template<nByte C> void read(typename EnableIf<C==TypeCode::BYTEARRAY, nByte*>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode::OBJECT, Object*>::type& retVal);
				template<nByte C> void read(typename EnableIf<C==TypeCode::UNKNOWN, Object>::type& retVal);
			private:
				template<nByte C> bool popArray(typename EnableIf<!TypeCode::IsValid1DArrayElement<C>::is, Object>::type& object, typename ArrayLengthType<typename TypeCode::TypeForCode<C>::type*>::type size);
				template<nByte C> bool popArray(typename EnableIf<TypeCode::IsValid1DArrayElement<C>::is&&C!=TypeCode::STRING&&C!=TypeCode::DICTIONARY&&C!=TypeCode::CUSTOM, Object>::type& object, typename ArrayLengthType<typename TypeCode::TypeForCode<C>::type*>::type size);
				template<nByte C> bool popArray(typename EnableIf<C==TypeCode::STRING, Object>::type& object, short size);
				template<nByte C> bool popArray(typename EnableIf<C==TypeCode::DICTIONARY, Object>::type& object, short size);
				template<nByte C> bool popArray(typename EnableIf<C==TypeCode::CUSTOM, Object>::type& object, short size);
				bool popType(nByte typeCode, Object& object);
				void popArrayOfDictionaries(Object& dict, short arraySize, int dimensions=0);
				const DictionaryBase* popDictionaryBody(nByte* pKeyTypes, nByte* pValueTypes, unsigned int* pDimensionsOfNestedValues, nByte customType);
				void readDictionaryTypes(nByte** ppKeyTypes, nByte** ppValTypes, unsigned int** ppDimensionsOfNestedValues, nByte* pCustomType);
			};



			template<nByte C>
			bool DeserializerImplementationProtocol16::pop(typename EnableIf<!TypeCode::IsValid<C>::is, Object>::type& /*object*/)
			{
				EGLOG(DebugLevel::ERRORS, L"unsupported type code %hd", C);
				return false;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::pop(typename EnableIf<TypeCode::IsValid<C>::is&&C!=TypeCode::EG_NULL&&C!=TypeCode::BYTEARRAY&&C!=TypeCode::OBJECT&&C!=TypeCode::ARRAY&&C!=TypeCode::STRING&&C!=TypeCode::DICTIONARY&&C!=TypeCode::CUSTOM&&C!=TypeCode::UNKNOWN, Object>::type& object)
			{
				typedef typename TypeCode::TypeForCode<C>::type T;
				typedef typename Conditional<IsArithmetic<T>::is, T, T& >::type TR;
				TR data = alloc<T>();
				read<C>(data);
				object.set(&data, C, 0, IsArithmetic<T>::is);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::pop(typename EnableIf<C==TypeCode::EG_NULL, Object>::type& object)
			{
				object.setToNULL();
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::pop(typename EnableIf<C==TypeCode::BYTEARRAY, Object>::type& object)
			{
				return popArray<TypeCode::BYTE>(object, read<TypeCode::INTEGER>());
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::pop(typename EnableIf<C==TypeCode::OBJECT, Object>::type& object)
			{
				short size = read<TypeCode::SHORT>();
				Object* arr = MemoryManagement::allocateArray<Object>(size);
				for(int i=0; i<size; ++i)
					popType(read<TypeCode::BYTE>(), arr[i]);
				object.set(arr, TypeCode::OBJECT, 0, size, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::pop(typename EnableIf<C==TypeCode::ARRAY, Object>::type& object)
			{
				short size = read<TypeCode::SHORT>();
				nByte type = read<TypeCode::BYTE>();
				switch(type)
				{
				case TypeCode::ARRAY:
					{
						typedef typename TypeCode::TypeForCode<C>::type T;
						Object innerArray;
						int* sizes;
						T* arr = MemoryManagement::allocateArray<T>(size);
						for(short i=0; i<size; ++i)
						{
							pop<TypeCode::ARRAY>(innerArray);
							arr[i] = const_cast<T>(innerArray.getData());
							if(i == size-1)
								break; // wait with deleting the last of the innerArray's, until we have have extracted type, dimensions and sizes (they will be identical for all inner arrays)
							innerArray.setDataNoCopy(NULL); // prevent the destructor of innerArray at the end of it's scope from freeing the payload, so that we can just copy the address above, as recursive deep-copies of arrays can be very expensive
							innerArray.setType(TypeCode::EG_NULL); // part 2 of payload-freeing prevention
						}

						sizes = MemoryManagement::allocateArray<int>(innerArray.getDimensions()+1);
						sizes[0] = size;
						for(unsigned int j=1; j<(innerArray.getDimensions()+1); ++j)
							sizes[j] = innerArray.getSizes()[j-1];
						object.set(arr, innerArray.getType(), innerArray.getCustomType(), innerArray.getDimensions()+1, sizes, false);
						innerArray.setDataNoCopy(NULL);
						innerArray.setType(TypeCode::EG_NULL);
						MemoryManagement::deallocateArray(sizes);
					}
					return true;
				default:
					switch(type)
					{
						CASES8(popArray, object, size) // expands to cases 0-255 with 'return popArray<caseNumber>(object, size)' getting called in each case
					}
					return false; // just to silence the "not all control paths return a value" compiler warning - this line could only ever be reached if the code would get compiled on a platform on which a byte is not 8bit
				}
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::pop(typename EnableIf<C==TypeCode::STRING, Object>::type& object)
			{
				short size = read<TypeCode::SHORT>();
				JString& data = *MemoryManagement::allocateArray<JString>(1, size);
				read<C>(data, size);
				object.set(&data, C, 0, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::pop(typename EnableIf<C==TypeCode::DICTIONARY, Object>::type& object)
			{
				popArrayOfDictionaries(object, 1);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::pop(typename EnableIf<C==TypeCode::CUSTOM, Object>::type& object, nByte customTypeCode)
			{
				if(!customTypeCode)
					customTypeCode = read<TypeCode::BYTE>();
				CustomTypeBase& data = *CustomTypeBase::allocObject(1, customTypeCode);
				read<C>(data, customTypeCode);
				object.set(&data, C, customTypeCode, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::pop(typename EnableIf<C==TypeCode::UNKNOWN, Object>::type& object)
			{
				return popType(read<TypeCode::BYTE>(), object);
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::popArray(typename EnableIf<!TypeCode::IsValid1DArrayElement<C>::is, Object>::type& /*object*/, typename ArrayLengthType<typename TypeCode::TypeForCode<C>::type*>::type /*size*/)
			{
				EGLOG(DebugLevel::ERRORS, L"unsupported type");
				return false;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::popArray(typename EnableIf<TypeCode::IsValid1DArrayElement<C>::is&&C!=TypeCode::STRING&&C!=TypeCode::DICTIONARY&&C!=TypeCode::CUSTOM, Object>::type& object, typename ArrayLengthType<typename TypeCode::TypeForCode<C>::type*>::type size)
			{
				typedef typename TypeCode::TypeForCode<C>::type T;
				T* arr = MemoryManagement::allocateArray<T>(size);
				for(typename ArrayLengthType<T*>::type i=0; i<size; ++i)
					read<C>(arr[i]);
				object.set(arr, C, 0, size, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::popArray(typename EnableIf<C==TypeCode::STRING, Object>::type& object, short size)
			{
				short sizeOfBiggestString = 0;
				int oldDataOffset = mDataOffset;
				for(short i=0; i<size; ++i)
				{
					short strSize = read<TypeCode::SHORT>();
					if(strSize>sizeOfBiggestString)
						sizeOfBiggestString = strSize;
					mDataOffset += strSize;
				}
				mDataOffset = oldDataOffset;
				JString* arr = MemoryManagement::allocateArray<JString>(size, sizeOfBiggestString);
				for(short i=0; i<size; ++i)
				{
					short strSize = read<TypeCode::SHORT>();
					arr[i].setFromUTF8Characters(reinterpret_cast<const char*>(mpData+mDataOffset), strSize);
					mDataOffset += strSize;
				}
				object.set(arr, C, 0, size, false);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::popArray(typename EnableIf<C==TypeCode::DICTIONARY, Object>::type& object, short size)
			{
				popArrayOfDictionaries(object, size, 1);
				return true;
			}

			template<nByte C>
			bool DeserializerImplementationProtocol16::popArray(typename EnableIf<C==TypeCode::CUSTOM, Object>::type& object, short size)
			{
				nByte customTypeCode = read<TypeCode::BYTE>();
				CustomTypeBase* arr = CustomTypeBase::allocObject(size, customTypeCode);
				for(short i=0; i<size; ++i)
					read<C>(*reinterpret_cast<CustomTypeBase*>(reinterpret_cast<nByte*>(arr)+CustomTypeBase::getSizeof(customTypeCode)*i), customTypeCode);
				object.set(arr, C, customTypeCode, size, false);
				return true;
			}

			template<nByte C>
			typename TypeCode::TypeForCode<C>::type DeserializerImplementationProtocol16::read(typename EnableIf<IsArithmetic<typename TypeCode::TypeForCode<C>::type>::is>::type*)
			{
				typename TypeCode::TypeForCode<C>::type retVal;
				read<C>(retVal);
				return retVal;
			}

			template<nByte C>
			void DeserializerImplementationProtocol16::read(typename EnableIf<C==TypeCode::BYTE||C==TypeCode::BOOLEAN, typename TypeCode::TypeForCode<C>::type>::type& retVal)
			{
				retVal = *reinterpret_cast<const typename TypeCode::TypeForCode<C>::type*>(mpData+(mDataOffset++));
			}

			template<nByte C>
			void DeserializerImplementationProtocol16::read(typename EnableIf<C==TypeCode::SHORT||C==TypeCode::INTEGER||C==TypeCode::LONG||C==TypeCode::FLOAT||C==TypeCode::DOUBLE, typename TypeCode::TypeForCode<C>::type>::type& retVal)
			{
				typedef typename TypeCode::TypeForCode<C>::type T;
#ifdef _EG_BIGENDIAN_PLATFORM
				MEMCPY(&retVal, mpData+mDataOffset, sizeof(T));
				mDataOffset += sizeof(T);
#else
				for(int i=sizeof(T)-1; i>=0; --i)
					reinterpret_cast<nByte*>(&retVal)[i] = read<TypeCode::BYTE>();
#endif
			}

			template<nByte C>
			void DeserializerImplementationProtocol16::read(typename EnableIf<C==TypeCode::STRING, JString>::type& retVal, short size)
			{
				retVal.setFromUTF8Characters(reinterpret_cast<const char*>(mpData+mDataOffset), size);
				mDataOffset += size;
			}

			template<nByte C>
			void DeserializerImplementationProtocol16::read(typename EnableIf<C==TypeCode::HASHTABLE, Hashtable>::type& retVal)
			{
				short size = read<TypeCode::SHORT>();
				for(short i=0; i<size; ++i)
				{
					Object k, v;
					pop(k);
					pop(v);
					retVal.template put<Object, Object>(k, v);
				}
			}

			template<nByte C>
			void DeserializerImplementationProtocol16::read(typename EnableIf<C==TypeCode::CUSTOM, CustomTypeBase>::type& retVal, nByte customTypeCode)
			{
				short length = read<TypeCode::SHORT>();
				CustomTypeBase::deserialize(mpData+mDataOffset, length, reinterpret_cast<void*>(&retVal), customTypeCode);
				mDataOffset += length;
			}

			template<nByte C>
			void DeserializerImplementationProtocol16::read(typename EnableIf<C==TypeCode::BYTEARRAY, nByte*>::type& retVal)
			{
				Object object;
				pop<TypeCode::BYTEARRAY>(object);
				retVal = static_cast<nByte*>(const_cast<void*>(object.getData()));
				object.setDataNoCopy(NULL); // prevent the destructor of object at the end of it's scope from freeing the payload, so that we can just copy the address into retVal
				object.setType(TypeCode::EG_NULL); // part 2 of payload-freeing prevention
			}

			template<nByte C>
			void DeserializerImplementationProtocol16::read(typename EnableIf<C==TypeCode::OBJECT, Object*>::type& retVal)
			{
				Object object;
				pop<TypeCode::OBJECT>(object);
				retVal = static_cast<Object*>(const_cast<void*>(object.getData()));
				object.setDataNoCopy(NULL); // prevent the destructor of object at the end of it's scope from freeing the payload, so that we can just copy the address into retVal
				object.setType(TypeCode::EG_NULL); // part 2 of payload-freeing prevention
			}

			template<nByte C>
			void DeserializerImplementationProtocol16::read(typename EnableIf<C==TypeCode::UNKNOWN, Object>::type& retVal)
			{
				pop(retVal);
			}
		}
	}
}
