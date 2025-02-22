/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * https://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Helpers/Stringification/StringifierImplementation.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			template<typename T>
			struct Stringifier : public StringifierImplementation<T, Helpers::IsDerivedFrom<T, Helpers::SmartPointerBase>::is>
			{
			};
		}
	}
}