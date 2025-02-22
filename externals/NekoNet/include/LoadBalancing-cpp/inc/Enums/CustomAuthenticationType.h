/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Common.h"

namespace ExitGames
{
	namespace LoadBalancing
	{
		/** Options for optional "Custom Authentication" services used with Photon. Used when the client sends an authentication request to the server.*/
		namespace CustomAuthenticationType
		{
			static const nByte CUSTOM          =  0; ///<Use a custom authentication service.
			static const nByte STEAM           =  1; ///<Authenticates users by their Steam Account. Pass L"ticket=[ticket]" to setParameters().
			static const nByte FACEBOOK        =  2; ///<Authenticates users by their Facebook Account. Pass L"token=[token]" to setParameters().
			static const nByte OCULUS          =  3; ///<Authenticates users by their Oculus Account. Pass L"userid=[userid]&nonce=[nonce]" to setParameters().
			static const nByte PLAYSTATION_4   =  4; ///<Authenticates users by their PSN Account. Pass L"token=[token]&env=[env]&userName=[userName]" to setParameters().
			static const nByte XBOX            =  5; ///<Authenticates users by their XBox Network Account. Pass the XSTS token to setData().
			static const nByte VIVEPORT        = 10; ///<Authenticates users by their HTC Viveport Account and user token. Pass L"userToken=[userToken]" to setParameters().
			static const nByte NINTENDO_SWITCH = 11; ///<Authenticates users by their Nintendo Account. Pass L"token=[token]&appversion=[appversion]" to setParameters(). The appversion is optional.
			static const nByte PLAYSTATION_5   = 12; ///<Authenticates users by their PSN Account. Pass L"token=[token]&env=[env]&userName=[userName]" to setParameters().
			static const nByte EPIC            = 13; ///<Authenticates users by their Epic Online Services (EOS) Account. Pass L"token=[token]&ownershipToken=[ownershipToken]" to setParameters(). The ownershipToken is optional.
			static const nByte FACEBOOK_GAMING = 15; ///<Authenticates users by their Facebook Account. Pass L"token=[token]" to setParameters().
			static const nByte NONE            = UCHAR_MAX; ///<Disables custom authentication.
		}
		/** @file */
	}
}