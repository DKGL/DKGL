//
//  File: RenderCommandEncoder.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2015-2017 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../GraphicsAPI.h"
#if DKGL_USE_DIRECT3D
#include <wrl.h>
#include <D3D12.h>
#include <dxgi1_5.h>
using Microsoft::WRL::ComPtr;

#include "../../DKRenderCommandEncoder.h"
#include "CommandBuffer.h"

namespace DKFramework
{
	namespace Private
	{
		namespace Direct3D
		{
			class RenderCommandEncoder : public DKRenderCommandEncoder
			{
			public:
				RenderCommandEncoder(ID3D12CommandList*);
				~RenderCommandEncoder(void);


				void EndEncoding(void) override;
				DKGraphicsDevice* Device(void) override;

				ComPtr<ID3D12CommandList> commandList;
				DKObject<CommandBuffer> commandBuffer;
			};
		}
	}
}

#endif //#if DKGL_USE_DIRECT3D