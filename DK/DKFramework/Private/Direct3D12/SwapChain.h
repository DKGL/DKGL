//
//  File: SwapChain.h
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

#include "../../DKWindow.h"
#include "CommandQueue.h"

namespace DKFramework
{
	namespace Private
	{
		namespace Direct3D
		{
			class SwapChain : public DKSwapChain
			{
			public:
				SwapChain(CommandQueue*, DKWindow*);
				~SwapChain(void);

				bool Setup(void);

				bool Present(void) override;

				DKObject<DKWindow> window;
				DKObject<CommandQueue> queue;

				ComPtr<IDXGISwapChain4> swapChain;

			private:
				void OnWindowEvent(const DKWindow::WindowEvent&);
			};
		}
	}
}

#endif //#if DKGL_USE_DIRECT3D