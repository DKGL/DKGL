//
//  File: ComputeCommandEncoder.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2015-2017 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../GraphicsAPI.h"
#if DKGL_USE_DIRECT3D
#include "d3d12_headers.h"

#include "../../DKComputeCommandEncoder.h"
#include "CommandBuffer.h"

namespace DKFramework
{
	namespace Private
	{
		namespace Direct3D
		{
			class ComputeCommandEncoder : public DKComputeCommandEncoder
			{
			public:
				ComputeCommandEncoder(ID3D12GraphicsCommandList*, CommandBuffer*);
				~ComputeCommandEncoder(void);


				void EndEncoding(void) override;
				DKCommandBuffer* Buffer(void) override;

				ComPtr<ID3D12GraphicsCommandList> commandList;
				DKObject<CommandBuffer> commandBuffer;
			};
		}
	}
}

#endif //#if DKGL_USE_DIRECT3D