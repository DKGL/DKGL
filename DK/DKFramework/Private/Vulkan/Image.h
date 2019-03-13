//
//  File: Image.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2015-2019 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../GraphicsAPI.h"
#if DKGL_ENABLE_VULKAN
#include <vulkan/vulkan.h>
#include "../../DKGraphicsDevice.h"
#include "../../DKTexture.h"
#include "Types.h"
#include "DeviceMemory.h"

namespace DKFramework::Private::Vulkan
{
    class Image
    {
    public:
        Image(DeviceMemory*, VkImage, const VkImageCreateInfo&);
        Image();
        ~Image();

        uint32_t Width() const 
        {
            return extent.width;
        }
        uint32_t Height() const 
        {
            return extent.height;
        }
        uint32_t Depth() const 
        {
            return extent.depth;
        }
        uint32_t MipmapCount() const 
        {
            return mipLevels;
        }
        uint32_t ArrayLength() const 
        {
            return arrayLayers;
        }
        DKTexture::Type TextureType() const 
        {
            switch (imageType)
            {
            case VK_IMAGE_TYPE_1D:
                return DKTexture::Type1D;
            case VK_IMAGE_TYPE_2D:
                return DKTexture::Type2D;
            case VK_IMAGE_TYPE_3D:
                return DKTexture::Type3D;
            }
            return DKTexture::TypeUnknown;
        }
        DKPixelFormat PixelFormat() const 
        {
            return Vulkan::PixelFormat(format);
        }

        VkImage					image;
        VkImageType				imageType;
        VkFormat				format;
        VkExtent3D				extent;
        uint32_t				mipLevels;
        uint32_t				arrayLayers;
        VkImageUsageFlags		usage;

        DKObject<DeviceMemory>  deviceMemory;
    };
}

#endif //#if DKGL_ENABLE_VULKAN
