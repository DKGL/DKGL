//
//  File: BlitCommandEncoder.mm
//  Platform: OS X, iOS
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2015-2017 Hongtae Kim. All rights reserved.
//

#include "../GraphicsAPI.h"
#if DKGL_USE_METAL

#include "BlitCommandEncoder.h"

using namespace DKFramework;
using namespace DKFramework::Private::Metal;

BlitCommandEncoder::BlitCommandEncoder(CommandBuffer* b, id<MTLBlitCommandEncoder> e)
: buffer(b)
, encoder(nil)
{
	encoder = [e retain];
}

BlitCommandEncoder::~BlitCommandEncoder(void)
{
	if (encoder)
		[encoder release];
}

void BlitCommandEncoder::EndEncoding(void)
{
	if (encoder)
	{
		[encoder endEncoding];
		[encoder release];
		encoder = nil;

		buffer->EndEncoder(this);
	}
}

DKCommandBuffer* BlitCommandEncoder::Buffer(void)
{
	return buffer;
}

#endif //#if DKGL_USE_METAL
