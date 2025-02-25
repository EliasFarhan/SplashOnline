/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Enums/SerializationProtocol.h"
#include "Common-cpp/inc/Helpers/ValueToObject.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			class SerializerImplementation;
		}

		class Serializer : public Base
		{
		public:
			using ToString::toString;

			Serializer(nByte protocol=SerializationProtocol::DEFAULT);
			~Serializer(void);

			const nByte* getData(void) const;
			int getSize(void) const;

			template<typename T> bool push(const T& data);
			template<typename T> bool push(const T pData, int arraySize);
			template<typename T> bool push(const T pData, const int* arraySizes);

			virtual JString& toString(JString& retStr, bool withTypes=false) const;
		private:
			Helpers::SerializerImplementation& mImp;
		};

		
		
		/**
		   Adds a serialized representation of parameter data to the Serializer-instance on which it is called.
		   @tparam T type of parameter data - has to be of one of the @link Datatypes supported datatypes\endlink
		   @param data data to serialize
		   @returns true if successful, false in case of an error */
		template<typename T>
		bool Serializer::push(const T& data)
		{
			typename Helpers::ConfirmAllowed<T>::type forceCompilationToFailForUnsupportedTypes; (void)forceCompilationToFailForUnsupportedTypes;
			COMPILE_TIME_ASSERT2_TRUE_MSG(!Helpers::ConfirmAllowed<typename Helpers::ConfirmAllowed<T>::type>::dimensions, ERROR_THIS_OVERLOAD_IS_ONLY_FOR_SINGLE_VALUES);
			return mImp.push(data, true);
		}
		
		/**
		   @overload
		   @details
		   This overload accepts singledimensional arrays and NULL-pointers passed for parameter pData.
		   @param pData array of data to serialize
		   @param arraySize the size of the value array */
		template<typename T>
		bool Serializer::push(const T pData, int arraySize)
		{
			typename Helpers::ConfirmAllowed<T>::type forceCompilationToFailForUnsupportedTypes; (void)forceCompilationToFailForUnsupportedTypes;
			COMPILE_TIME_ASSERT2_TRUE_MSG(Helpers::ConfirmAllowed<typename Helpers::ConfirmAllowed<T>::type>::dimensions==1, ERROR_THIS_OVERLOAD_IS_ONLY_FOR_1D_ARRAYS);
			return mImp.push(pData, arraySize, true);
		}
		
		/**
		   @overload
		   @details
		   This overload accepts multidimensional arrays and NULL-pointers passed for parameter val.
		   The array, passed for parameter pData has to be a pointer of the correct abstraction level, meaning a normal pointer for
		   a singledimensional array, a doublepointer for a twodimensional array, a triplepointer for a threedimensional array and so on.
		   @param pData array of data to serialize
		   @param arraySizes the sizes for every dimension of the array  */
		template<typename T>
		bool Serializer::push(const T pData, const int* arraySizes)
		{
			typename Helpers::ConfirmAllowed<T>::type forceCompilationToFailForUnsupportedTypes; (void)forceCompilationToFailForUnsupportedTypes;
			COMPILE_TIME_ASSERT2_TRUE_MSG((bool)Helpers::ConfirmAllowed<typename Helpers::ConfirmAllowed<T>::type>::dimensions, ERROR_THIS_OVERLOAD_IS_ONLY_FOR_FOR_ARRAYS);
			return mImp.push(pData, arraySizes, true);
		}
	}
}