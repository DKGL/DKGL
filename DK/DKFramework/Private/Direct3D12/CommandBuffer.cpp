//
//  File: CommandBuffer.cpp
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2015-2017 Hongtae Kim. All rights reserved.
//

#include "../GraphicsAPI.h"
#if DKGL_ENABLE_DIRECT3D12

#include "CommandBuffer.h"
#include "CommandQueue.h"
#include "GraphicsDevice.h"
#include "RenderCommandEncoder.h"
#include "ComputeCommandEncoder.h"
#include "CopyCommandEncoder.h"

using namespace DKFramework;
using namespace DKFramework::Private::Direct3D;


CommandBuffer::CommandBuffer(CommandAllocator* a, DKCommandQueue* q)
	: commandAllocator(a)
	, queue(q)
{
}

CommandBuffer::~CommandBuffer()
{
	GraphicsDevice* dev = (GraphicsDevice*)DKGraphicsDeviceInterface::Instance(this->Device());
	dev->ReleaseCommandAllocator(this->commandAllocator);

	for (ComPtr<ID3D12GraphicsCommandList>& list : commandLists)
	{
		dev->ReleaseCommandList(list.Get());
	}
}

DKObject<DKRenderCommandEncoder> CommandBuffer::CreateRenderCommandEncoder(const DKRenderPassDescriptor& desc)
{
	GraphicsDevice* dev = (GraphicsDevice*)DKGraphicsDeviceInterface::Instance(this->Device());
	ComPtr<ID3D12GraphicsCommandList> cm = dev->GetCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
	cm->Reset(commandAllocator->allocator.Get(), nullptr);

	DKObject<RenderCommandEncoder> encoder = DKOBJECT_NEW RenderCommandEncoder(cm.Get(), this, desc);
	return encoder.SafeCast<DKRenderCommandEncoder>();
}

DKObject<DKComputeCommandEncoder> CommandBuffer::CreateComputeCommandEncoder()
{
	GraphicsDevice* dev = (GraphicsDevice*)DKGraphicsDeviceInterface::Instance(this->Device());
	ComPtr<ID3D12GraphicsCommandList> cm = dev->GetCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
	cm->Reset(commandAllocator->allocator.Get(), nullptr);

	DKObject<ComputeCommandEncoder> encoder = DKOBJECT_NEW ComputeCommandEncoder(cm.Get(), this);
	return encoder.SafeCast<DKComputeCommandEncoder>();
}

DKObject<DKCopyCommandEncoder> CommandBuffer::CreateCopyCommandEncoder()
{
	GraphicsDevice* dev = (GraphicsDevice*)DKGraphicsDeviceInterface::Instance(this->Device());
	ComPtr<ID3D12GraphicsCommandList> cm = dev->GetCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
	cm->Reset(commandAllocator->allocator.Get(), nullptr);

	DKObject<CopyCommandEncoder> encoder = DKOBJECT_NEW CopyCommandEncoder(cm.Get(), this);
	return encoder.SafeCast<DKCopyCommandEncoder>();
}

void CommandBuffer::FinishCommandList(ID3D12GraphicsCommandList* list)
{
	commandLists.Add(list);
}

bool CommandBuffer::Commit()
{
	if (commandLists.Count() > 0 && commandAllocator->IsCompleted())
	{
		CommandQueue* commandQueue = this->queue.StaticCast<CommandQueue>();
		ComPtr<ID3D12GraphicsCommandList>* lists = this->commandLists;
		UINT numLists = static_cast<UINT>(this->commandLists.Count());

		static_assert(sizeof(ComPtr<ID3D12GraphicsCommandList>) == sizeof(ID3D12CommandList*), "");

		UINT64 enqueued = commandQueue->Enqueue(reinterpret_cast<ID3D12CommandList * const *>(lists), numLists);
		this->commandAllocator->SetPendingState(commandQueue->fence.Get(), enqueued);
		return true;
	}
	return false;
}

bool CommandBuffer::WaitUntilCompleted()
{
	return commandAllocator->WaitUntilCompleted(INFINITE);
}

#endif //#if DKGL_ENABLE_DIRECT3D12
