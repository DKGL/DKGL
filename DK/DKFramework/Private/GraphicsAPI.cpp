//
//  File: GraphicsAPI.cpp
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2004-2017 Hongtae Kim. All rights reserved.
//

#include <initializer_list>
#include "../DKPropertySet.h"
#include "../Interface/DKGraphicsDeviceInterface.h"
#include "GraphicsAPI.h"

namespace DKFramework
{
	namespace Private
	{
#if DKGL_ENABLE_METAL
		namespace Metal
		{
			DKGraphicsDeviceInterface* CreateInterface();
		}
#endif
#if DKGL_ENABLE_VULKAN
		namespace Vulkan
		{
			DKGraphicsDeviceInterface* CreateInterface();
		}
#endif
#if DKGL_ENABLE_DIRECT3D12
        namespace Direct3D
        {
            DKGraphicsDeviceInterface* CreateInterface();
        }
#endif
	}

	DKGraphicsDeviceInterface* DKGraphicsDeviceInterface::CreateInterface()
	{
		DKString defaultAPI =
#if DKGL_ENABLE_METAL
		"Metal";
#elif DKGL_ENABLE_VULKAN
		"Vulkan";
#elif DKGL_ENABLE_DIRECT3D12
		"Direct3D";
#else
		"";
#endif
		struct APISet
		{
			const char* name;
			DKGraphicsDeviceInterface* (*fn)();
			bool preferred;
		};

        DKArray<APISet> apis = {
#if DKGL_ENABLE_METAL
            { "Metal", Private::Metal::CreateInterface, false },
#endif
#if DKGL_ENABLE_VULKAN
            { "Vulkan", Private::Vulkan::CreateInterface, false },
#endif            
#if DKGL_ENABLE_DIRECT3D12
            { "Direct3D", Private::Direct3D::CreateInterface, false},
#endif
        };

		for (size_t i = 0; i < apis.Count(); ++i)
		{
			if (defaultAPI.CompareNoCase(apis.Value(i).name) == 0)
			{
				if (i > 0)
				{
					APISet api = apis.Value(i);
					apis.Remove(i);
					apis.Insert(api, 0);
				}
				break;
			}
		}

		// get user preferred API name.
		DKPropertySet::SystemConfig().LookUpValueForKeyPath(preferredGraphicsAPIKey,
															DKFunction([&apis](const DKVariant& var)->bool
		{
			if (var.ValueType() == DKVariant::TypeString)
			{
				DKString selectAPI = var.String();

				for (size_t i = 0; i < apis.Count(); ++i)
				{
					if (selectAPI.CompareNoCase(apis.Value(i).name) == 0)
					{
						if (i > 0)
						{
							APISet api = apis.Value(i);
							api.preferred = true;
							apis.Remove(i);
							apis.Insert(api, 0);
						}
						break;
					}
				}
				return true;
			}
			return false;
		}));


		bool printApiSet = true;
		if (printApiSet)
		{
			int index = 0;
			for (const APISet& as : apis)
			{
				DKLogI(" GraphicsAPI[%d]: \"%s\"%s", index, as.name, as.preferred ? " (Preferred)" : "");
				index++;
			}
			if (index == 0)
				DKLogW(" No Graphics API available.");
		}

		for (const APISet& as : apis)
		{
			try {
				DKGraphicsDeviceInterface* device = as.fn();
				if (device)
				{
					DKLogI("Graphics API: \"%s\", Device: \"%ls\" selected.", as.name, (const wchar_t*)device->DeviceName());
					return device;
				}
				else
				{
					DKLogW("Graphics API \"%s\" not supported.", as.name);
				}
			}
			catch (DKError& e)
			{
				DKLogE("Graphics API \"%s\" Failed: %ls", as.name, (const wchar_t*)e.Description());
			}
			catch (std::exception& e)
			{
				DKLogE("Graphics API \"%s\" Failed: %s", as.name, e.what());
			}
		}

		DKLogE("ERROR: No Graphics device found.");
		return NULL;
	}
}
