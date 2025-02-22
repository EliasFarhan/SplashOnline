/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/platformLayer.h"
#if defined _EG_WINDOWSSTORE_PLATFORM || defined _EG_XB1_PLATFORM || defined _EG_SWITCH_PLATFORM || defined _EG_GAMECORE_PLATFORM
#	include <thread>
#else
#	include "Common-cpp/inc/porting.h"
#endif

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			namespace Thread
			{
				class ID
				{
				public:
					ID(void);

					bool operator==(const ID& Rhs) const;
					bool operator!=(const ID& Rhs) const;
				private:
#			if defined _EG_UNIX_PLATFORM && !defined _EG_SWITCH_PLATFORM
					ID(pthread_t id);
					pthread_t mID;
#			elif defined _EG_MICROSOFT_PLATFORM
					ID(unsigned long id);
					unsigned long mID;
#			else
					ID(std::thread::id id);
					std::thread::id mID;
#			endif

					friend ID getLocalThreadID(void);
				};

				typedef void (*callbackSignature)(void* pArg);

				void create(callbackSignature onEnterThread, void* pArg);

				ID getLocalThreadID(void);

				// weakly linked - the empty default implementations can be overridden by the app code, in case the app code needs to run any special code whenever the execution of a photon thread starts or ends
				void onEnterThread(void);
				void onLeaveThread(void);
			}
		}
	}
}