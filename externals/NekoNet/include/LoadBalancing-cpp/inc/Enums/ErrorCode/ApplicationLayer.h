/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

namespace ExitGames
{
	namespace LoadBalancing
	{
		namespace ErrorCode
		{
			static const int OK                                      =     0; // Success

			namespace ApplicationLayer
			{
				static const int OK                                  =     0; // Success

				static const int INVALID_AUTHENTICATION              = 32767;
				static const int GAME_ID_ALREADY_EXISTS              = 32766;
				static const int GAME_FULL                           = 32765;
				static const int GAME_CLOSED                         = 32764;
				static const int ALREADY_MATCHED                     = 32763;
				static const int SERVER_FULL                         = 32762;
				static const int USER_BLOCKED                        = 32761;
				static const int NO_MATCH_FOUND                      = 32760;
				static const int GAME_DOES_NOT_EXIST                 = 32758;
				static const int MAX_CCU_REACHED                     = 32757;
				static const int INVALID_REGION                      = 32756;
				static const int CUSTOM_AUTHENTICATION_FAILED        = 32755;
				static const int AUTHENTICATION_TOKEN_EXPIRED        = 32753;
				static const int PLUGIN_REPORTED_ERROR               = 32752;
				static const int PLUGIN_MISMATCH                     = 32751;
				static const int JOIN_FAILED_PEER_ALREADY_JOINED     = 32750;
				static const int JOIN_FAILED_FOUND_INACTIVE_JOINER   = 32749;
				static const int JOIN_FAILED_WITH_REJOINER_NOT_FOUND = 32748;
				static const int JOIN_FAILED_FOUND_EXCLUDED_USER_ID  = 32747;
				static const int JOIN_FAILED_FOUND_ACTIVE_JOINER     = 32746;
				static const int HTTP_LIMIT_REACHED                  = 32745;
				static const int EXTERNAL_HTTP_CALL_FAILED           = 32744;
				static const int SLOT_ERROR                          = 32742;
				static const int INVALID_ENCRYPTION_PARAMETERS       = 32741;
				static const int CLIENT_VERSION_TOO_OLD              = 32667;
				static const int CLIENT_VERSION_INVALID              = 32666;
				static const int DASHBOARD_VERSION_INVALID           = 32665;
			}
		}
		/** @file */
	}
}