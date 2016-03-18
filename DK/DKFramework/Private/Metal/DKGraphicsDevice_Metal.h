//
//  File: DKGraphicsDevice_Metal.h
//  Platform: OS X, iOS
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2015-2016 Hongtae Kim. All rights reserved.
//


#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>

#include "../../Interface/DKGraphicsDeviceInterface.h"

using namespace DKFoundation;

namespace DKFramework
{
	namespace Private
	{
		class DKGraphicsDevice_Metal : public DKGraphicsDeviceInterface
		{
		public:
			~DKGraphicsDevice_Metal(void);
		};
	}
}

#endif
