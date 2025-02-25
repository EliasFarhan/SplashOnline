/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/CustomTypeBase.h"
#include "Common-cpp/inc/Helpers/TypeTraits/Conditional.h"
#include "Common-cpp/inc/Helpers/TypeTraits/EnableIf.h"
#include "Common-cpp/inc/Helpers/TypeTraits/IsArithmetic.h"
#include "Common-cpp/inc/Helpers/TypeTraits/IsDerivedFrom.h"
#include "Common-cpp/inc/MemoryManagement/Allocate.h"
#include "Common-cpp/inc/Object.h"

// a macro for each bit
#define CASES0(n, f, ...) case n: return f<n>(__VA_ARGS__);
#define CASES1(n, f, ...) CASES0(n, f, __VA_ARGS__) CASES0(n+1, f, __VA_ARGS__)
#define CASES2(n, f, ...) CASES1(n, f, __VA_ARGS__) CASES1(n+(1<<1), f, __VA_ARGS__)
#define CASES3(n, f, ...) CASES2(n, f, __VA_ARGS__) CASES2(n+(1<<2), f, __VA_ARGS__)
#define CASES4(n, f, ...) CASES3(n, f, __VA_ARGS__) CASES3(n+(1<<3), f, __VA_ARGS__)
#define CASES5(n, f, ...) CASES4(n, f, __VA_ARGS__) CASES4(n+(1<<4), f, __VA_ARGS__)
#define CASES6(n, f, ...) CASES5(n, f, __VA_ARGS__) CASES5(n+(1<<5), f, __VA_ARGS__)
#define CASES7(n, f, ...) CASES6(n, f, __VA_ARGS__) CASES6(n+(1<<6), f, __VA_ARGS__)
#define CASES8(f, ...) CASES7(0, f, __VA_ARGS__) CASES7((1<<7), f, __VA_ARGS__)

// a macro for each bit
#define CASESC0(n, c, m) case n: return c<n>::m;
#define CASESC1(n, c, m) CASESC0(n, c, m) CASESC0(n+1, c, m)
#define CASESC2(n, c, m) CASESC1(n, c, m) CASESC1(n+(1<<1), c, m)
#define CASESC3(n, c, m) CASESC2(n, c, m) CASESC2(n+(1<<2), c, m)
#define CASESC4(n, c, m) CASESC3(n, c, m) CASESC3(n+(1<<3), c, m)
#define CASESC5(n, c, m) CASESC4(n, c, m) CASESC4(n+(1<<4), c, m)
#define CASESC6(n, c, m) CASESC5(n, c, m) CASESC5(n+(1<<5), c, m)
#define CASESC7(n, c, m) CASESC6(n, c, m) CASESC6(n+(1<<6), c, m)
#define CASESC8(c, m) CASESC7(0, c, m) CASESC7((1<<7), c, m)

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			class DeserializerImplementation : public Base
			{
			public:
				DeserializerImplementation(const nByte* data, unsigned int size);
				virtual ~DeserializerImplementation(void);

				int getReadPosition(void) const;

				virtual bool pop(Object& object) = 0;
				virtual bool popByte(Object& object) = 0;

				virtual nByte readByte(void) = 0;
				virtual short readShort(void) = 0;

				virtual JString& toString(JString& retStr, bool withTypes=false) const;
			protected:
				template<typename T> T alloc(typename EnableIf<IsArithmetic<T>::is>::type* = NULL);
				template<typename T> T& alloc(typename EnableIf<!IsArithmetic<T>::is>::type* = NULL);

				const nByte* mpData;
				unsigned int mSize;
				unsigned int mDataOffset;
			};



			template<typename T>
			T DeserializerImplementation::alloc(typename EnableIf<IsArithmetic<T>::is>::type*)
			{
				return T();
			}

			template<typename T>
			T& DeserializerImplementation::alloc(typename EnableIf<!IsArithmetic<T>::is>::type*)
			{
				return *MemoryManagement::allocateArray<T>(1);
			}
		}
	}
}
