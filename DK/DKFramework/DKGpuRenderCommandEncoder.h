//
//  File: DKGpuRenderCommandEncoder.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2015-2016 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../DKInclude.h"
#include "../DKFoundation.h"

////////////////////////////////////////////////////////////////////////////////
// DKGpuRenderCommandEncoder
////////////////////////////////////////////////////////////////////////////////

namespace DKFramework
{
	class DKGpuRenderCommandEncoder
	{
	public:
		virtual ~DKGpuRenderCommandEncoder(void) {}
	};

	class DKParallelGpuRenderCommandEncoder
	{
	public:
		virtual ~DKParallelGpuRenderCommandEncoder(void) {}
		virtual DKFoundation::DKObject<DKGpuRenderCommandEncoder> CreateEncoder(void) = 0;
	};
}
