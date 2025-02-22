/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Common.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			template<typename T>
			class Retainable : public T
			{
			public:
				using T::operator=;

				                                                                          Retainable(void)                                                                 :                         mRefCount(0) {retain();}
				template<typename P1>                                                     Retainable(      typename EnableIf<!IsDerivedFrom<P1, T>::is, P1>::type& p1)     : T(p1)                 , mRefCount(0) {retain();}
				template<typename P1>                                                     Retainable(const typename EnableIf<!IsDerivedFrom<P1, T>::is, P1>::type& p1)     : T(p1)                 , mRefCount(0) {retain();}
				template<typename P1, typename P2>                                        Retainable(      P1& p1,       P2& p2)                                           : T(p1, p2)             , mRefCount(0) {retain();}
				template<typename P1, typename P2>                                        Retainable(      P1& p1, const P2& p2)                                           : T(p1, p2)             , mRefCount(0) {retain();}
				template<typename P1, typename P2>                                        Retainable(const P1& p1,       P2& p2)                                           : T(p1, p2)             , mRefCount(0) {retain();}
				template<typename P1, typename P2>                                        Retainable(const P1& p1, const P2& p2)                                           : T(p1, p2)             , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3>                           Retainable(      P1& p1,       P2& p2,       P3& p3)                             : T(p1, p2, p3)         , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3>                           Retainable(      P1& p1,       P2& p2, const P3& p3)                             : T(p1, p2, p3)         , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3>                           Retainable(      P1& p1, const P2& p2,       P3& p3)                             : T(p1, p2, p3)         , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3>                           Retainable(      P1& p1, const P2& p2, const P3& p3)                             : T(p1, p2, p3)         , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3>                           Retainable(const P1& p1,       P2& p2,       P3& p3)                             : T(p1, p2, p3)         , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3>                           Retainable(const P1& p1,       P2& p2, const P3& p3)                             : T(p1, p2, p3)         , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3>                           Retainable(const P1& p1, const P2& p2,       P3& p3)                             : T(p1, p2, p3)         , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3>                           Retainable(const P1& p1, const P2& p2, const P3& p3)                             : T(p1, p2, p3)         , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(      P1& p1,       P2& p2,       P3& p3,       P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(      P1& p1,       P2& p2,       P3& p3, const P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(      P1& p1,       P2& p2, const P3& p3,       P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(      P1& p1,       P2& p2, const P3& p3, const P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(      P1& p1, const P2& p2,       P3& p3,       P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(      P1& p1, const P2& p2,       P3& p3, const P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(      P1& p1, const P2& p2, const P3& p3,       P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(      P1& p1, const P2& p2, const P3& p3, const P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(const P1& p1,       P2& p2,       P3& p3,       P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(const P1& p1,       P2& p2,       P3& p3, const P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(const P1& p1,       P2& p2, const P3& p3,       P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(const P1& p1,       P2& p2, const P3& p3, const P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(const P1& p1, const P2& p2,       P3& p3,       P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(const P1& p1, const P2& p2,       P3& p3, const P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(const P1& p1, const P2& p2, const P3& p3,       P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4>              Retainable(const P1& p1, const P2& p2, const P3& p3, const P4& p4)               : T(p1, p2, p3, p4)     , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1,       P2& p2,       P3& p3,       P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1,       P2& p2,       P3& p3,       P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1,       P2& p2,       P3& p3, const P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1,       P2& p2,       P3& p3, const P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1,       P2& p2, const P3& p3,       P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1,       P2& p2, const P3& p3,       P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1,       P2& p2, const P3& p3, const P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1,       P2& p2, const P3& p3, const P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1, const P2& p2,       P3& p3,       P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1, const P2& p2,       P3& p3,       P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1, const P2& p2,       P3& p3, const P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1, const P2& p2,       P3& p3, const P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1, const P2& p2, const P3& p3,       P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1, const P2& p2, const P3& p3,       P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1, const P2& p2, const P3& p3, const P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(      P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1,       P2& p2,       P3& p3,       P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1,       P2& p2,       P3& p3,       P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1,       P2& p2,       P3& p3, const P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1,       P2& p2,       P3& p3, const P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1,       P2& p2, const P3& p3,       P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1,       P2& p2, const P3& p3,       P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1,       P2& p2, const P3& p3, const P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1,       P2& p2, const P3& p3, const P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1, const P2& p2,       P3& p3,       P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1, const P2& p2,       P3& p3,       P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1, const P2& p2,       P3& p3, const P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1, const P2& p2,       P3& p3, const P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1, const P2& p2, const P3& p3,       P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1, const P2& p2, const P3& p3,       P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1, const P2& p2, const P3& p3, const P4& p4,       P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}
				template<typename P1, typename P2, typename P3, typename P4, typename P5> Retainable(const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) : T(p1, p2, p3, p4, p5) , mRefCount(0) {retain();}

				Retainable<T>(const Retainable<T>& toCopy);
				Retainable<T>(const T& toCopy);
				virtual Retainable<T>& operator=(const Retainable<T>& toCopy);
				virtual Retainable<T>& operator=(const T& toCopy);

				virtual Retainable<T>* retain(void);
				virtual const Retainable<T>* retain(void) const;
				virtual void release(void) const;

				virtual unsigned char getRefCount(void) const;
			protected:
				virtual ~Retainable(void);
			private:
#ifdef EG_PLATFORM_SUPPORTS_ATOMICS
				mutable std::atomic<unsigned char> mRefCount;
#else
				mutable unsigned char mRefCount;
#endif
			};



			template<typename T>
			Retainable<T>::Retainable(const Retainable<T>& toCopy)
			{
				*this = toCopy;
			}

			template<typename T>
			Retainable<T>::Retainable(const T& toCopy)
			{
				*this = toCopy;
			}

			template<typename T>
			Retainable<T>& Retainable<T>::operator=(const Retainable<T>& toCopy)
			{
				return Retainable<T>::operator=(static_cast<const T&>(toCopy));
			}

			template<typename T>
			Retainable<T>& Retainable<T>::operator=(const T& toCopy)
			{
				T::operator=(toCopy);
				mRefCount = 0;
				retain();
				return *this;
			}

			template<typename T>
			Retainable<T>::~Retainable(void)
			{
			}

			template<typename T>
			Retainable<T>* Retainable<T>::retain(void)
			{
				++mRefCount;
				return this;
			}

			template<typename T>
			const Retainable<T>* Retainable<T>::retain(void) const
			{
				++mRefCount;
				return this;
			}

			template<typename T>
			void Retainable<T>::release(void) const
			{
				if(!--mRefCount)
					DEALLOCATE(Retainable<T>, this);
			}

			template<typename T>
			unsigned char Retainable<T>::getRefCount(void) const
			{
				return mRefCount;
			}
		}
	}
}