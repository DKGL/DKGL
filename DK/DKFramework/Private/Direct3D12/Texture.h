//
//  File: Texture.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2015-2017 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../GraphicsAPI.h"
#if DKGL_USE_DIRECT3D
#include "d3d12_headers.h"

#include "../../DKTexture.h"
#include "TextureBaseT.h"

namespace DKFramework
{
	namespace Private
	{
		namespace Direct3D
		{
			class GraphicsDevice;
			class Texture : public TextureBaseT<DKTexture>
			{
			public:
				Texture(GraphicsDevice*, ID3D12Resource*, D3D12_CPU_DESCRIPTOR_HANDLE);
				~Texture(void);

				GraphicsDevice* device;
				D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;
			};
		}
	}
}

#endif //#if DKGL_USE_DIRECT3D