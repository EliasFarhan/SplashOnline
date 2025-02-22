/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Enums/SerializationProtocol.h"
#include "Photon-cpp/inc/Enums/ConnectionProtocol.h"
#include "LoadBalancing-cpp/inc/Enums/RegionSelectionMode.h"

namespace ExitGames
{
	namespace LoadBalancing
	{
		class ClientConstructOptions : public Common::ToString
		{
		public:
			using ToString::toString;

			ClientConstructOptions(nByte connectionProtocol=Photon::ConnectionProtocol::DEFAULT, bool autoLobbyStats=false, nByte regionSelectionMode=RegionSelectionMode::DEFAULT, bool useAlternativePorts=false, nByte serializationProtocol=Common::SerializationProtocol::DEFAULT);
			~ClientConstructOptions(void);

			ClientConstructOptions(const ClientConstructOptions& toCopy);
			ClientConstructOptions& operator=(const ClientConstructOptions& toCopy);

			nByte getConnectionProtocol(void) const;
			ClientConstructOptions& setConnectionProtocol(nByte connectionProtocol);
			bool getAutoLobbyStats(void) const;
			ClientConstructOptions& setAutoLobbyStats(bool autoLobbyStats);
			nByte getRegionSelectionMode(void) const;
			ClientConstructOptions& setRegionSelectionMode(nByte regionSelectionMode);
			bool getUseAlternativePorts(void) const;
			ClientConstructOptions& setUseAlternativePorts(bool useAlternativePorts);
			nByte getSerializationProtocol(void) const;
			ClientConstructOptions& setSerializationProtocol(nByte serializationProtocol);

			virtual Common::JString& toString(Common::JString& retStr, bool withTypes=false) const;
		private:
			nByte mConnectionProtocol;
			bool mAutoLobbyStats;
			nByte mRegionSelectionMode;
			bool mUseAlternativePorts;
			nByte mSerializationProtocol;
		};
	}
}