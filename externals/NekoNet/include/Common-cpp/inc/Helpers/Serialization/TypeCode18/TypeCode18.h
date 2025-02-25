/* Exit Games Photon Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/defines.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			namespace TypeCode18
			{
				static const nByte UNKNOWN                =    0;
				static const nByte BOOLEAN                =    2;
				static const nByte BYTE                   =    3;
				static const nByte SHORT                  =    4;
				static const nByte FLOAT                  =    5;
				static const nByte DOUBLE                 =    6;
				static const nByte STRING                 =    7;
				static const nByte EG_NULL                =    8;
				static const nByte COMPRESSED_INT         =    9;
				static const nByte COMPRESSED_LONG        =   10;
				static const nByte INT1                   =   11;
				static const nByte INT1_                  =   12;
				static const nByte INT2                   =   13;
				static const nByte INT2_                  =   14;
				static const nByte L1                     =   15;
				static const nByte L1_                    =   16;
				static const nByte L2                     =   17;
				static const nByte L2_                    =   18;
				static const nByte CUSTOM                 =   19;
				static const nByte CUSTOM_TYPE_SLIM       = 0x80;
				static const nByte CUSTOM_TYPE_SLIM_RANGE =  100;
				static const nByte CUSTOM_TYPE_SLIM_MAX   = CUSTOM_TYPE_SLIM | CUSTOM_TYPE_SLIM_RANGE; // not defined in protocol, only used locally to determine the correct pop-specialization for slim custom types
				static const nByte DICTIONARY             =   20;
				static const nByte HASHTABLE              =   21;
				static const nByte OBJECT_ARRAY           =   23;
				static const nByte OPERATION_REQUEST      =   24;
				static const nByte OPERATION_RESPONSE     =   25;
				static const nByte EVENT_DATA             =   26;
				static const nByte BOOLEAN_FALSE          =   27;
				static const nByte BOOLEAN_TRUE           =   28;
				static const nByte SHORT_ZERO             =   29;
				static const nByte INT_ZERO               =   30;
				static const nByte LONG_ZERO              =   31;
				static const nByte FLOAT_ZERO             =   32;
				static const nByte DOUBLE_ZERO            =   33;
				static const nByte BYTE_ZERO              =   34;
				static const nByte ARRAY                  = 0x40;
				static const nByte BOOLEAN_ARRAY          = BOOLEAN | ARRAY;
				static const nByte BYTE_ARRAY             = BYTE | ARRAY;
				static const nByte SHORT_ARRAY            = SHORT | ARRAY;
				static const nByte FLOAT_ARRAY            = FLOAT | ARRAY;
				static const nByte DOUBLE_ARRAY           = DOUBLE | ARRAY;
				static const nByte STRING_ARRAY           = STRING | ARRAY;
				static const nByte COMPRESSED_INT_ARRAY   = COMPRESSED_INT | ARRAY;
				static const nByte COMPRESSED_LONG_ARRAY  = COMPRESSED_LONG | ARRAY;
				static const nByte CUSTOM_TYPE_ARRAY      = CUSTOM | ARRAY;
				static const nByte DICTIONARY_ARRAY       = DICTIONARY | ARRAY;
				static const nByte HASHTABLE_ARRAY        = HASHTABLE | ARRAY;
				static const nByte COMPRESSED_UINT        =  254; // not defined in protocol, only used locally with TypeForCode to have it return an unsigned int
				static const nByte COMPRESSED_ULONG       =  255; // not defined in protocol, only used locally with TypeForCode to have it return an uint64
			}
		}

		/** @file */
	}
}