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
			namespace Core
			{
				static const int OK                                  =     0; // Success

				static const int INTERNAL_SERVER_ERROR               =    -1; // In case of an exception during the connection-process or the handling of an operation.
				static const int OPERATION_INVALID                   =    -2; // The operation has an unexpected format.
				static const int OPERATION_DENIED                    =    -3; // The operation is not allowed in the current state.
				static const int ARGUMENT_OUT_OF_RANGE               =    -4;
				static const int INVALID_REQUEST_PARAMETERS          =    -6;

				// connection related error codes:
				static const int NOT_READY                           =    -7; // The client tried to connect too early. The server is not ready yet.
				static const int OVERLOAD                            =    -8; // The server is overloaded.
				static const int BACKOFF                             =    -9; // The client has to wait for 10 seconds before the next try.
				static const int MAINTENANCE                         =   -10; // The server is under Maintenance.

				// other errors:
				static const int SEND_BUFFER_FULL                    =   -11; // The client handles incoming data too slow and because of that the server side send buffer got overloaded
				static const int UNEXPECTED_DATA                     =   -12; // Data from the client has an invalid format.
				static const int SERIALIZATION_LIMIT_ERROR           =   -13; // Data is valid from the protocol point of view but is not valid from the application logic point of view. For example when the client attempts to set a Hashtable with too many elements as a player or a room property
				static const int WRONG_INIT_REQUEST_DATA             =   -14; // In case of something unexpected in the Init request. Used only with InitV3.
				static const int RESPONSE_PARSE_ERROR                =   -15; // For S2S communication. Should never be received by the client.
				static const int CRYPTO_PROVIDER_NOT_SET             =   -16; // The server got encrypted data (PayloadEncryption) but no crypto provider has been set.
				static const int DECRYPTION_FAILURE                  =   -17; // Failure during decryption of data.
				static const int INVALID_ENCRYPTION_PARAMETERS       =   -18; // Invalid encryption parameters were provided by the client.
				static const int INCOMING_DATA_RATE_EXCEEDED         =   -30; // The maximum data rate for all data that is received by the server has been exceeded.
				static const int INCOMING_MSG_RATE_EXCEEDED          =   -31; // The maximum message rate for all messages that are received by the server has been exceeded.
				static const int INCOMING_MAX_MSG_SIZE_EXCEEDED      =   -32; // The maximum message size has been exceeded.
				static const int OPERATION_MAX_COUNT_EXCEEDED        =   -34; // The total count of operation requests was exceeded.
				static const int OPERATION_RATE_EXCEEDED             =   -35; // The maximum allowed amount of operations per second for the requested operation type has been exceeded
				static const int OPERATION_DATA_RATE_EXCEEDED        =   -36; // The maximum allowed amount of bytes per second for the requested operation type has been exceeded
				static const int OPERATION_BLOCKED                   =   -37; // The requested operation is not allowed on this server.
				static const int OPERATION_SIZE_LIMIT_EXCEEDED       =   -38; // The maximum size for an operation request has been exceeded.
				static const int OPERATION_PARAMETERS_LIMIT_EXCEEDED =   -39; // A limit specific to the requested operation type has been exceeded.
				static const int MESSAGES_RATE_EXCEEDED              =   -40; // The maximum allowed amount of messages per second has been exceeded
				static const int MESSAGES_DATA_RATE_EXCEEDED         =   -41; // The maximum allowed amount of bytes per second for messages has been exceeded
				static const int MESSAGES_BLOCKED                    =   -42; // Messages are blocked on this server
				static const int MESSAGE_SIZE_LIMIT_EXCEEDED         =   -43; // The max message size has been exceeded
			}
		}
		/** @file */
	}
}