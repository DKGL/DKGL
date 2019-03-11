//
//  File: CopyCommandEncoder.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2016-2019 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../GraphicsAPI.h"
#if DKGL_ENABLE_VULKAN
#include <vulkan/vulkan.h>

#include "../../DKCopyCommandEncoder.h"
#include "CommandBuffer.h"

namespace DKFramework::Private::Vulkan
{
    class CopyCommandEncoder : public DKCopyCommandEncoder
    {
        struct EncodingState
        {
        };
        using EncoderCommand = DKFunctionSignature<void(VkCommandBuffer, EncodingState&)>;
        class Encoder : public CommandBufferEncoder
        {
        public:
            Encoder(class CommandBuffer*);
            ~Encoder();

            bool Encode(VkCommandBuffer) override;

            // Retain ownership of all encoded objects
            DKArray<DKObject<DKGpuBuffer>> buffers;
            DKArray<DKObject<DKTexture>> textures;

            class CommandBuffer* commandBuffer;
            DKArray<DKObject<EncoderCommand>> commands;
        };
        DKObject<Encoder> encoder;

    public:
        CopyCommandEncoder(class CommandBuffer*);

        void EndEncoding() override;
        bool IsCompleted() const override { return encoder == nullptr; }
        DKCommandBuffer* CommandBuffer() override { return commandBuffer; }

        void CopyFromBufferToBuffer(DKGpuBuffer* src, size_t srcOffset,
                                    DKGpuBuffer* dst, size_t dstOffset,
                                    size_t size) override;

        void CopyFromBufferToTexture(DKGpuBuffer* src, const BufferImageOrigin& srcOffset,
                                     DKTexture* dst, const TextureOrigin& dstOffset,
                                     const Size& size) override;
        void CopyFromTextureToBuffer(DKTexture* src, const TextureOrigin& srcOffset,
                                     DKGpuBuffer* dst, const BufferImageOrigin& dstOffset,
                                     const Size& size) override;

        void CopyFromTextureToTexture(DKTexture* src, const TextureOrigin& srcOffset,
                                      DKTexture* dst, const TextureOrigin& dstOffset,
                                      const Size& size) override;

        void FillBuffer(DKGpuBuffer* buffer, size_t offset, size_t length, uint8_t value) override;

        DKObject<class CommandBuffer> commandBuffer;

    private:
        static void SetupSubresource(const TextureOrigin& origin,
                                     uint32_t layerCount,
                                     DKPixelFormat pixelFormat,
                                     VkImageSubresourceLayers& subresource);
    };
}
#endif //#if DKGL_ENABLE_VULKAN
