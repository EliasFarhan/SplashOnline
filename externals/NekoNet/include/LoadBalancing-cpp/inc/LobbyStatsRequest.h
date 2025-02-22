/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "LoadBalancing-cpp/inc/Enums/LobbyType.h"

namespace ExitGames
{
	namespace LoadBalancing
	{
		class LobbyStatsRequest : public Common::ToString
		{
		public:
			using ToString::toString;

			LobbyStatsRequest(const Common::JString& name=Common::JString(), nByte type=LobbyType::DEFAULT);

			const Common::JString& getName(void) const;
			nByte getType(void) const;

			virtual Common::JString& toString(Common::JString& retStr, bool withTypes=false) const;
		private:
			Common::JString mName;
			nByte mType;
		};
	}
}