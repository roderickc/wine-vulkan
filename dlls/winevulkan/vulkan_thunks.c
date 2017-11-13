/* Automatically generated from Vulkan vk.xml; DO NOT EDIT! */

#include "config.h"
#include "wine/port.h"

#include "wine/debug.h"
#include "wine/vulkan.h"
#include "wine/vulkan_driver.h"
#include "vulkan_private.h"

WINE_DEFAULT_DEBUG_CHANNEL(vulkan);

static VkResult WINAPI wine_vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex)
{
    TRACE("%p, 0x%s, 0x%s, 0x%s, 0x%s, %p\n", device, wine_dbgstr_longlong(swapchain), wine_dbgstr_longlong(timeout), wine_dbgstr_longlong(semaphore), wine_dbgstr_longlong(fence), pImageIndex);
    return device->funcs.p_vkAcquireNextImageKHR(device->device, swapchain, timeout, semaphore, fence, pImageIndex);
}

static VkResult WINAPI wine_vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo *pAllocateInfo, VkDescriptorSet *pDescriptorSets)
{
    TRACE("%p, %p, %p\n", device, pAllocateInfo, pDescriptorSets);
    return device->funcs.p_vkAllocateDescriptorSets(device->device, pAllocateInfo, pDescriptorSets);
}

static VkResult WINAPI wine_vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo *pAllocateInfo, const VkAllocationCallbacks *pAllocator, VkDeviceMemory *pMemory)
{
    TRACE("%p, %p, %p, %p\n", device, pAllocateInfo, pAllocator, pMemory);
    return device->funcs.p_vkAllocateMemory(device->device, pAllocateInfo, NULL, pMemory);
}

static VkResult WINAPI wine_vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo *pBeginInfo)
{
    TRACE("%p, %p\n", commandBuffer, pBeginInfo);
    return commandBuffer->device->funcs.p_vkBeginCommandBuffer(commandBuffer->command_buffer, pBeginInfo);
}

static VkResult WINAPI wine_vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
    TRACE("%p, 0x%s, 0x%s, 0x%s\n", device, wine_dbgstr_longlong(buffer), wine_dbgstr_longlong(memory), wine_dbgstr_longlong(memoryOffset));
    return device->funcs.p_vkBindBufferMemory(device->device, buffer, memory, memoryOffset);
}

static VkResult WINAPI wine_vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
    TRACE("%p, 0x%s, 0x%s, 0x%s\n", device, wine_dbgstr_longlong(image), wine_dbgstr_longlong(memory), wine_dbgstr_longlong(memoryOffset));
    return device->funcs.p_vkBindImageMemory(device->device, image, memory, memoryOffset);
}

static void WINAPI wine_vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags)
{
    TRACE("%p, 0x%s, %u, %#x\n", commandBuffer, wine_dbgstr_longlong(queryPool), query, flags);
    commandBuffer->device->funcs.p_vkCmdBeginQuery(commandBuffer->command_buffer, queryPool, query, flags);
}

static void WINAPI wine_vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo *pRenderPassBegin, VkSubpassContents contents)
{
    TRACE("%p, %p, %d\n", commandBuffer, pRenderPassBegin, contents);
    commandBuffer->device->funcs.p_vkCmdBeginRenderPass(commandBuffer->command_buffer, pRenderPassBegin, contents);
}

static void WINAPI wine_vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets)
{
    TRACE("%p, %d, 0x%s, %u, %u, %p, %u, %p\n", commandBuffer, pipelineBindPoint, wine_dbgstr_longlong(layout), firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
    commandBuffer->device->funcs.p_vkCmdBindDescriptorSets(commandBuffer->command_buffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

static void WINAPI wine_vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
    TRACE("%p, 0x%s, 0x%s, %d\n", commandBuffer, wine_dbgstr_longlong(buffer), wine_dbgstr_longlong(offset), indexType);
    commandBuffer->device->funcs.p_vkCmdBindIndexBuffer(commandBuffer->command_buffer, buffer, offset, indexType);
}

static void WINAPI wine_vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
    TRACE("%p, %d, 0x%s\n", commandBuffer, pipelineBindPoint, wine_dbgstr_longlong(pipeline));
    commandBuffer->device->funcs.p_vkCmdBindPipeline(commandBuffer->command_buffer, pipelineBindPoint, pipeline);
}

static void WINAPI wine_vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer *pBuffers, const VkDeviceSize *pOffsets)
{
    TRACE("%p, %u, %u, %p, %p\n", commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
    commandBuffer->device->funcs.p_vkCmdBindVertexBuffers(commandBuffer->command_buffer, firstBinding, bindingCount, pBuffers, pOffsets);
}

static void WINAPI wine_vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit *pRegions, VkFilter filter)
{
    TRACE("%p, 0x%s, %d, 0x%s, %d, %u, %p, %d\n", commandBuffer, wine_dbgstr_longlong(srcImage), srcImageLayout, wine_dbgstr_longlong(dstImage), dstImageLayout, regionCount, pRegions, filter);
    commandBuffer->device->funcs.p_vkCmdBlitImage(commandBuffer->command_buffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
}

static void WINAPI wine_vkCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment *pAttachments, uint32_t rectCount, const VkClearRect *pRects)
{
    TRACE("%p, %u, %p, %u, %p\n", commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
    commandBuffer->device->funcs.p_vkCmdClearAttachments(commandBuffer->command_buffer, attachmentCount, pAttachments, rectCount, pRects);
}

static void WINAPI wine_vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue *pColor, uint32_t rangeCount, const VkImageSubresourceRange *pRanges)
{
    TRACE("%p, 0x%s, %d, %p, %u, %p\n", commandBuffer, wine_dbgstr_longlong(image), imageLayout, pColor, rangeCount, pRanges);
    commandBuffer->device->funcs.p_vkCmdClearColorImage(commandBuffer->command_buffer, image, imageLayout, pColor, rangeCount, pRanges);
}

static void WINAPI wine_vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue *pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange *pRanges)
{
    TRACE("%p, 0x%s, %d, %p, %u, %p\n", commandBuffer, wine_dbgstr_longlong(image), imageLayout, pDepthStencil, rangeCount, pRanges);
    commandBuffer->device->funcs.p_vkCmdClearDepthStencilImage(commandBuffer->command_buffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}

static void WINAPI wine_vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy *pRegions)
{
    TRACE("%p, 0x%s, 0x%s, %u, %p\n", commandBuffer, wine_dbgstr_longlong(srcBuffer), wine_dbgstr_longlong(dstBuffer), regionCount, pRegions);
    commandBuffer->device->funcs.p_vkCmdCopyBuffer(commandBuffer->command_buffer, srcBuffer, dstBuffer, regionCount, pRegions);
}

static void WINAPI wine_vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
    TRACE("%p, 0x%s, 0x%s, %d, %u, %p\n", commandBuffer, wine_dbgstr_longlong(srcBuffer), wine_dbgstr_longlong(dstImage), dstImageLayout, regionCount, pRegions);
    commandBuffer->device->funcs.p_vkCmdCopyBufferToImage(commandBuffer->command_buffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}

static void WINAPI wine_vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy *pRegions)
{
    TRACE("%p, 0x%s, %d, 0x%s, %d, %u, %p\n", commandBuffer, wine_dbgstr_longlong(srcImage), srcImageLayout, wine_dbgstr_longlong(dstImage), dstImageLayout, regionCount, pRegions);
    commandBuffer->device->funcs.p_vkCmdCopyImage(commandBuffer->command_buffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

static void WINAPI wine_vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
    TRACE("%p, 0x%s, %d, 0x%s, %u, %p\n", commandBuffer, wine_dbgstr_longlong(srcImage), srcImageLayout, wine_dbgstr_longlong(dstBuffer), regionCount, pRegions);
    commandBuffer->device->funcs.p_vkCmdCopyImageToBuffer(commandBuffer->command_buffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
}

static void WINAPI wine_vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
    TRACE("%p, 0x%s, %u, %u, 0x%s, 0x%s, 0x%s, %#x\n", commandBuffer, wine_dbgstr_longlong(queryPool), firstQuery, queryCount, wine_dbgstr_longlong(dstBuffer), wine_dbgstr_longlong(dstOffset), wine_dbgstr_longlong(stride), flags);
    commandBuffer->device->funcs.p_vkCmdCopyQueryPoolResults(commandBuffer->command_buffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

static void WINAPI wine_vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    TRACE("%p, %u, %u, %u\n", commandBuffer, groupCountX, groupCountY, groupCountZ);
    commandBuffer->device->funcs.p_vkCmdDispatch(commandBuffer->command_buffer, groupCountX, groupCountY, groupCountZ);
}

static void WINAPI wine_vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
    TRACE("%p, 0x%s, 0x%s\n", commandBuffer, wine_dbgstr_longlong(buffer), wine_dbgstr_longlong(offset));
    commandBuffer->device->funcs.p_vkCmdDispatchIndirect(commandBuffer->command_buffer, buffer, offset);
}

static void WINAPI wine_vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    TRACE("%p, %u, %u, %u, %u\n", commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    commandBuffer->device->funcs.p_vkCmdDraw(commandBuffer->command_buffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

static void WINAPI wine_vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    TRACE("%p, %u, %u, %u, %d, %u\n", commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    commandBuffer->device->funcs.p_vkCmdDrawIndexed(commandBuffer->command_buffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

static void WINAPI wine_vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    TRACE("%p, 0x%s, 0x%s, %u, %u\n", commandBuffer, wine_dbgstr_longlong(buffer), wine_dbgstr_longlong(offset), drawCount, stride);
    commandBuffer->device->funcs.p_vkCmdDrawIndexedIndirect(commandBuffer->command_buffer, buffer, offset, drawCount, stride);
}

static void WINAPI wine_vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    TRACE("%p, 0x%s, 0x%s, %u, %u\n", commandBuffer, wine_dbgstr_longlong(buffer), wine_dbgstr_longlong(offset), drawCount, stride);
    commandBuffer->device->funcs.p_vkCmdDrawIndirect(commandBuffer->command_buffer, buffer, offset, drawCount, stride);
}

static void WINAPI wine_vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query)
{
    TRACE("%p, 0x%s, %u\n", commandBuffer, wine_dbgstr_longlong(queryPool), query);
    commandBuffer->device->funcs.p_vkCmdEndQuery(commandBuffer->command_buffer, queryPool, query);
}

static void WINAPI wine_vkCmdEndRenderPass(VkCommandBuffer commandBuffer)
{
    TRACE("%p\n", commandBuffer);
    commandBuffer->device->funcs.p_vkCmdEndRenderPass(commandBuffer->command_buffer);
}

static void WINAPI wine_vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers)
{
    FIXME("stub: %p, %u, %p\n", commandBuffer, commandBufferCount, pCommandBuffers);
}

static void WINAPI wine_vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
    TRACE("%p, 0x%s, 0x%s, 0x%s, %u\n", commandBuffer, wine_dbgstr_longlong(dstBuffer), wine_dbgstr_longlong(dstOffset), wine_dbgstr_longlong(size), data);
    commandBuffer->device->funcs.p_vkCmdFillBuffer(commandBuffer->command_buffer, dstBuffer, dstOffset, size, data);
}

static void WINAPI wine_vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents)
{
    TRACE("%p, %d\n", commandBuffer, contents);
    commandBuffer->device->funcs.p_vkCmdNextSubpass(commandBuffer->command_buffer, contents);
}

static void WINAPI wine_vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers)
{
    TRACE("%p, %#x, %#x, %#x, %u, %p, %u, %p, %u, %p\n", commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    commandBuffer->device->funcs.p_vkCmdPipelineBarrier(commandBuffer->command_buffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

static void WINAPI wine_vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void *pValues)
{
    TRACE("%p, 0x%s, %#x, %u, %u, %p\n", commandBuffer, wine_dbgstr_longlong(layout), stageFlags, offset, size, pValues);
    commandBuffer->device->funcs.p_vkCmdPushConstants(commandBuffer->command_buffer, layout, stageFlags, offset, size, pValues);
}

static void WINAPI wine_vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
    TRACE("%p, 0x%s, %#x\n", commandBuffer, wine_dbgstr_longlong(event), stageMask);
    commandBuffer->device->funcs.p_vkCmdResetEvent(commandBuffer->command_buffer, event, stageMask);
}

static void WINAPI wine_vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
    TRACE("%p, 0x%s, %u, %u\n", commandBuffer, wine_dbgstr_longlong(queryPool), firstQuery, queryCount);
    commandBuffer->device->funcs.p_vkCmdResetQueryPool(commandBuffer->command_buffer, queryPool, firstQuery, queryCount);
}

static void WINAPI wine_vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve *pRegions)
{
    TRACE("%p, 0x%s, %d, 0x%s, %d, %u, %p\n", commandBuffer, wine_dbgstr_longlong(srcImage), srcImageLayout, wine_dbgstr_longlong(dstImage), dstImageLayout, regionCount, pRegions);
    commandBuffer->device->funcs.p_vkCmdResolveImage(commandBuffer->command_buffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

static void WINAPI wine_vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4])
{
    TRACE("%p, %p\n", commandBuffer, blendConstants);
    commandBuffer->device->funcs.p_vkCmdSetBlendConstants(commandBuffer->command_buffer, blendConstants);
}

static void WINAPI wine_vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    TRACE("%p, %f, %f, %f\n", commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
    commandBuffer->device->funcs.p_vkCmdSetDepthBias(commandBuffer->command_buffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

static void WINAPI wine_vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds)
{
    TRACE("%p, %f, %f\n", commandBuffer, minDepthBounds, maxDepthBounds);
    commandBuffer->device->funcs.p_vkCmdSetDepthBounds(commandBuffer->command_buffer, minDepthBounds, maxDepthBounds);
}

static void WINAPI wine_vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
    TRACE("%p, 0x%s, %#x\n", commandBuffer, wine_dbgstr_longlong(event), stageMask);
    commandBuffer->device->funcs.p_vkCmdSetEvent(commandBuffer->command_buffer, event, stageMask);
}

static void WINAPI wine_vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth)
{
    TRACE("%p, %f\n", commandBuffer, lineWidth);
    commandBuffer->device->funcs.p_vkCmdSetLineWidth(commandBuffer->command_buffer, lineWidth);
}

static void WINAPI wine_vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D *pScissors)
{
    TRACE("%p, %u, %u, %p\n", commandBuffer, firstScissor, scissorCount, pScissors);
    commandBuffer->device->funcs.p_vkCmdSetScissor(commandBuffer->command_buffer, firstScissor, scissorCount, pScissors);
}

static void WINAPI wine_vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask)
{
    TRACE("%p, %#x, %u\n", commandBuffer, faceMask, compareMask);
    commandBuffer->device->funcs.p_vkCmdSetStencilCompareMask(commandBuffer->command_buffer, faceMask, compareMask);
}

static void WINAPI wine_vkCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference)
{
    TRACE("%p, %#x, %u\n", commandBuffer, faceMask, reference);
    commandBuffer->device->funcs.p_vkCmdSetStencilReference(commandBuffer->command_buffer, faceMask, reference);
}

static void WINAPI wine_vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask)
{
    TRACE("%p, %#x, %u\n", commandBuffer, faceMask, writeMask);
    commandBuffer->device->funcs.p_vkCmdSetStencilWriteMask(commandBuffer->command_buffer, faceMask, writeMask);
}

static void WINAPI wine_vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport *pViewports)
{
    TRACE("%p, %u, %u, %p\n", commandBuffer, firstViewport, viewportCount, pViewports);
    commandBuffer->device->funcs.p_vkCmdSetViewport(commandBuffer->command_buffer, firstViewport, viewportCount, pViewports);
}

static void WINAPI wine_vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void *pData)
{
    TRACE("%p, 0x%s, 0x%s, 0x%s, %p\n", commandBuffer, wine_dbgstr_longlong(dstBuffer), wine_dbgstr_longlong(dstOffset), wine_dbgstr_longlong(dataSize), pData);
    commandBuffer->device->funcs.p_vkCmdUpdateBuffer(commandBuffer->command_buffer, dstBuffer, dstOffset, dataSize, pData);
}

static void WINAPI wine_vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent *pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers)
{
    TRACE("%p, %u, %p, %#x, %#x, %u, %p, %u, %p, %u, %p\n", commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    commandBuffer->device->funcs.p_vkCmdWaitEvents(commandBuffer->command_buffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

static void WINAPI wine_vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query)
{
    TRACE("%p, %d, 0x%s, %u\n", commandBuffer, pipelineStage, wine_dbgstr_longlong(queryPool), query);
    commandBuffer->device->funcs.p_vkCmdWriteTimestamp(commandBuffer->command_buffer, pipelineStage, queryPool, query);
}

static VkResult WINAPI wine_vkCreateBuffer(VkDevice device, const VkBufferCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkBuffer *pBuffer)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pBuffer);
    return device->funcs.p_vkCreateBuffer(device->device, pCreateInfo, NULL, pBuffer);
}

static VkResult WINAPI wine_vkCreateBufferView(VkDevice device, const VkBufferViewCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkBufferView *pView)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pView);
    return device->funcs.p_vkCreateBufferView(device->device, pCreateInfo, NULL, pView);
}

static VkResult WINAPI wine_vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkCommandPool *pCommandPool)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pCommandPool);
    return device->funcs.p_vkCreateCommandPool(device->device, pCreateInfo, NULL, pCommandPool);
}

static VkResult WINAPI wine_vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo *pCreateInfos, const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines)
{
    TRACE("%p, 0x%s, %u, %p, %p, %p\n", device, wine_dbgstr_longlong(pipelineCache), createInfoCount, pCreateInfos, pAllocator, pPipelines);
    return device->funcs.p_vkCreateComputePipelines(device->device, pipelineCache, createInfoCount, pCreateInfos, NULL, pPipelines);
}

static VkResult WINAPI wine_vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDescriptorPool *pDescriptorPool)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pDescriptorPool);
    return device->funcs.p_vkCreateDescriptorPool(device->device, pCreateInfo, NULL, pDescriptorPool);
}

static VkResult WINAPI wine_vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDescriptorSetLayout *pSetLayout)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pSetLayout);
    return device->funcs.p_vkCreateDescriptorSetLayout(device->device, pCreateInfo, NULL, pSetLayout);
}

static VkResult WINAPI wine_vkCreateEvent(VkDevice device, const VkEventCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkEvent *pEvent)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pEvent);
    return device->funcs.p_vkCreateEvent(device->device, pCreateInfo, NULL, pEvent);
}

static VkResult WINAPI wine_vkCreateFence(VkDevice device, const VkFenceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkFence *pFence)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pFence);
    return device->funcs.p_vkCreateFence(device->device, pCreateInfo, NULL, pFence);
}

static VkResult WINAPI wine_vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkFramebuffer *pFramebuffer)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pFramebuffer);
    return device->funcs.p_vkCreateFramebuffer(device->device, pCreateInfo, NULL, pFramebuffer);
}

static VkResult WINAPI wine_vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo *pCreateInfos, const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines)
{
    TRACE("%p, 0x%s, %u, %p, %p, %p\n", device, wine_dbgstr_longlong(pipelineCache), createInfoCount, pCreateInfos, pAllocator, pPipelines);
    return device->funcs.p_vkCreateGraphicsPipelines(device->device, pipelineCache, createInfoCount, pCreateInfos, NULL, pPipelines);
}

static VkResult WINAPI wine_vkCreateImage(VkDevice device, const VkImageCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkImage *pImage)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pImage);
    return device->funcs.p_vkCreateImage(device->device, pCreateInfo, NULL, pImage);
}

static VkResult WINAPI wine_vkCreateImageView(VkDevice device, const VkImageViewCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkImageView *pView)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pView);
    return device->funcs.p_vkCreateImageView(device->device, pCreateInfo, NULL, pView);
}

static VkResult WINAPI wine_vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkPipelineCache *pPipelineCache)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pPipelineCache);
    return device->funcs.p_vkCreatePipelineCache(device->device, pCreateInfo, NULL, pPipelineCache);
}

static VkResult WINAPI wine_vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkPipelineLayout *pPipelineLayout)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pPipelineLayout);
    return device->funcs.p_vkCreatePipelineLayout(device->device, pCreateInfo, NULL, pPipelineLayout);
}

static VkResult WINAPI wine_vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkQueryPool *pQueryPool)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pQueryPool);
    return device->funcs.p_vkCreateQueryPool(device->device, pCreateInfo, NULL, pQueryPool);
}

static VkResult WINAPI wine_vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkRenderPass *pRenderPass)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pRenderPass);
    return device->funcs.p_vkCreateRenderPass(device->device, pCreateInfo, NULL, pRenderPass);
}

static VkResult WINAPI wine_vkCreateSampler(VkDevice device, const VkSamplerCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSampler *pSampler)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pSampler);
    return device->funcs.p_vkCreateSampler(device->device, pCreateInfo, NULL, pSampler);
}

static VkResult WINAPI wine_vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSemaphore *pSemaphore)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pSemaphore);
    return device->funcs.p_vkCreateSemaphore(device->device, pCreateInfo, NULL, pSemaphore);
}

static VkResult WINAPI wine_vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkShaderModule *pShaderModule)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pShaderModule);
    return device->funcs.p_vkCreateShaderModule(device->device, pCreateInfo, NULL, pShaderModule);
}

static VkResult WINAPI wine_vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSwapchainKHR *pSwapchain)
{
    TRACE("%p, %p, %p, %p\n", device, pCreateInfo, pAllocator, pSwapchain);
    return device->funcs.p_vkCreateSwapchainKHR(device->device, pCreateInfo, NULL, pSwapchain);
}

static void WINAPI wine_vkDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(buffer), pAllocator);
    device->funcs.p_vkDestroyBuffer(device->device, buffer, NULL);
}

static void WINAPI wine_vkDestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(bufferView), pAllocator);
    device->funcs.p_vkDestroyBufferView(device->device, bufferView, NULL);
}

static void WINAPI wine_vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(commandPool), pAllocator);
    device->funcs.p_vkDestroyCommandPool(device->device, commandPool, NULL);
}

static void WINAPI wine_vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(descriptorPool), pAllocator);
    device->funcs.p_vkDestroyDescriptorPool(device->device, descriptorPool, NULL);
}

static void WINAPI wine_vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(descriptorSetLayout), pAllocator);
    device->funcs.p_vkDestroyDescriptorSetLayout(device->device, descriptorSetLayout, NULL);
}

static void WINAPI wine_vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(event), pAllocator);
    device->funcs.p_vkDestroyEvent(device->device, event, NULL);
}

static void WINAPI wine_vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(fence), pAllocator);
    device->funcs.p_vkDestroyFence(device->device, fence, NULL);
}

static void WINAPI wine_vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(framebuffer), pAllocator);
    device->funcs.p_vkDestroyFramebuffer(device->device, framebuffer, NULL);
}

static void WINAPI wine_vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(image), pAllocator);
    device->funcs.p_vkDestroyImage(device->device, image, NULL);
}

static void WINAPI wine_vkDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(imageView), pAllocator);
    device->funcs.p_vkDestroyImageView(device->device, imageView, NULL);
}

static void WINAPI wine_vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(pipeline), pAllocator);
    device->funcs.p_vkDestroyPipeline(device->device, pipeline, NULL);
}

static void WINAPI wine_vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(pipelineCache), pAllocator);
    device->funcs.p_vkDestroyPipelineCache(device->device, pipelineCache, NULL);
}

static void WINAPI wine_vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(pipelineLayout), pAllocator);
    device->funcs.p_vkDestroyPipelineLayout(device->device, pipelineLayout, NULL);
}

static void WINAPI wine_vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(queryPool), pAllocator);
    device->funcs.p_vkDestroyQueryPool(device->device, queryPool, NULL);
}

static void WINAPI wine_vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(renderPass), pAllocator);
    device->funcs.p_vkDestroyRenderPass(device->device, renderPass, NULL);
}

static void WINAPI wine_vkDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(sampler), pAllocator);
    device->funcs.p_vkDestroySampler(device->device, sampler, NULL);
}

static void WINAPI wine_vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(semaphore), pAllocator);
    device->funcs.p_vkDestroySemaphore(device->device, semaphore, NULL);
}

static void WINAPI wine_vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(shaderModule), pAllocator);
    device->funcs.p_vkDestroyShaderModule(device->device, shaderModule, NULL);
}

static void WINAPI wine_vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(swapchain), pAllocator);
    device->funcs.p_vkDestroySwapchainKHR(device->device, swapchain, NULL);
}

static VkResult WINAPI wine_vkDeviceWaitIdle(VkDevice device)
{
    TRACE("%p\n", device);
    return device->funcs.p_vkDeviceWaitIdle(device->device);
}

static VkResult WINAPI wine_vkEndCommandBuffer(VkCommandBuffer commandBuffer)
{
    TRACE("%p\n", commandBuffer);
    return commandBuffer->device->funcs.p_vkEndCommandBuffer(commandBuffer->command_buffer);
}

static VkResult WINAPI wine_vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char *pLayerName, uint32_t *pPropertyCount, VkExtensionProperties *pProperties)
{
    TRACE("%p, %p, %p, %p\n", physicalDevice, pLayerName, pPropertyCount, pProperties);
    return physicalDevice->instance->funcs.p_vkEnumerateDeviceExtensionProperties(physicalDevice->phys_dev, pLayerName, pPropertyCount, pProperties);
}

static VkResult WINAPI wine_vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
    TRACE("%p, %p, %p\n", physicalDevice, pPropertyCount, pProperties);
    return physicalDevice->instance->funcs.p_vkEnumerateDeviceLayerProperties(physicalDevice->phys_dev, pPropertyCount, pProperties);
}

static VkResult WINAPI wine_vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange *pMemoryRanges)
{
    TRACE("%p, %u, %p\n", device, memoryRangeCount, pMemoryRanges);
    return device->funcs.p_vkFlushMappedMemoryRanges(device->device, memoryRangeCount, pMemoryRanges);
}

static VkResult WINAPI wine_vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets)
{
    TRACE("%p, 0x%s, %u, %p\n", device, wine_dbgstr_longlong(descriptorPool), descriptorSetCount, pDescriptorSets);
    return device->funcs.p_vkFreeDescriptorSets(device->device, descriptorPool, descriptorSetCount, pDescriptorSets);
}

static void WINAPI wine_vkFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks *pAllocator)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(memory), pAllocator);
    device->funcs.p_vkFreeMemory(device->device, memory, NULL);
}

static void WINAPI wine_vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements *pMemoryRequirements)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(buffer), pMemoryRequirements);
    device->funcs.p_vkGetBufferMemoryRequirements(device->device, buffer, pMemoryRequirements);
}

static void WINAPI wine_vkGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory, VkDeviceSize *pCommittedMemoryInBytes)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(memory), pCommittedMemoryInBytes);
    device->funcs.p_vkGetDeviceMemoryCommitment(device->device, memory, pCommittedMemoryInBytes);
}

static VkResult WINAPI wine_vkGetEventStatus(VkDevice device, VkEvent event)
{
    TRACE("%p, 0x%s\n", device, wine_dbgstr_longlong(event));
    return device->funcs.p_vkGetEventStatus(device->device, event);
}

static VkResult WINAPI wine_vkGetFenceStatus(VkDevice device, VkFence fence)
{
    TRACE("%p, 0x%s\n", device, wine_dbgstr_longlong(fence));
    return device->funcs.p_vkGetFenceStatus(device->device, fence);
}

static void WINAPI wine_vkGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements *pMemoryRequirements)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(image), pMemoryRequirements);
    device->funcs.p_vkGetImageMemoryRequirements(device->device, image, pMemoryRequirements);
}

static void WINAPI wine_vkGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t *pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements *pSparseMemoryRequirements)
{
    TRACE("%p, 0x%s, %p, %p\n", device, wine_dbgstr_longlong(image), pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    device->funcs.p_vkGetImageSparseMemoryRequirements(device->device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

static void WINAPI wine_vkGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource *pSubresource, VkSubresourceLayout *pLayout)
{
    TRACE("%p, 0x%s, %p, %p\n", device, wine_dbgstr_longlong(image), pSubresource, pLayout);
    device->funcs.p_vkGetImageSubresourceLayout(device->device, image, pSubresource, pLayout);
}

static void WINAPI wine_vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures *pFeatures)
{
    TRACE("%p, %p\n", physicalDevice, pFeatures);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceFeatures(physicalDevice->phys_dev, pFeatures);
}

static void WINAPI wine_vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties *pFormatProperties)
{
    TRACE("%p, %d, %p\n", physicalDevice, format, pFormatProperties);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceFormatProperties(physicalDevice->phys_dev, format, pFormatProperties);
}

static VkResult WINAPI wine_vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties *pImageFormatProperties)
{
    TRACE("%p, %d, %d, %d, %#x, %#x, %p\n", physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
    return physicalDevice->instance->funcs.p_vkGetPhysicalDeviceImageFormatProperties(physicalDevice->phys_dev, format, type, tiling, usage, flags, pImageFormatProperties);
}

static void WINAPI wine_vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties *pMemoryProperties)
{
    TRACE("%p, %p\n", physicalDevice, pMemoryProperties);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceMemoryProperties(physicalDevice->phys_dev, pMemoryProperties);
}

static void WINAPI wine_vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *pProperties)
{
    TRACE("%p, %p\n", physicalDevice, pProperties);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceProperties(physicalDevice->phys_dev, pProperties);
}

static void WINAPI wine_vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t *pQueueFamilyPropertyCount, VkQueueFamilyProperties *pQueueFamilyProperties)
{
    TRACE("%p, %p, %p\n", physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->phys_dev, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

static void WINAPI wine_vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t *pPropertyCount, VkSparseImageFormatProperties *pProperties)
{
    TRACE("%p, %d, %d, %d, %#x, %d, %p, %p\n", physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
    physicalDevice->instance->funcs.p_vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice->phys_dev, format, type, samples, usage, tiling, pPropertyCount, pProperties);
}

static VkResult WINAPI wine_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities)
{
    TRACE("%p, 0x%s, %p\n", physicalDevice, wine_dbgstr_longlong(surface), pSurfaceCapabilities);
    return physicalDevice->instance->funcs.p_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->phys_dev, surface, pSurfaceCapabilities);
}

static VkResult WINAPI wine_vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats)
{
    TRACE("%p, 0x%s, %p, %p\n", physicalDevice, wine_dbgstr_longlong(surface), pSurfaceFormatCount, pSurfaceFormats);
    return physicalDevice->instance->funcs.p_vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->phys_dev, surface, pSurfaceFormatCount, pSurfaceFormats);
}

static VkResult WINAPI wine_vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes)
{
    TRACE("%p, 0x%s, %p, %p\n", physicalDevice, wine_dbgstr_longlong(surface), pPresentModeCount, pPresentModes);
    return physicalDevice->instance->funcs.p_vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->phys_dev, surface, pPresentModeCount, pPresentModes);
}

static VkResult WINAPI wine_vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32 *pSupported)
{
    TRACE("%p, %u, 0x%s, %p\n", physicalDevice, queueFamilyIndex, wine_dbgstr_longlong(surface), pSupported);
    return physicalDevice->instance->funcs.p_vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice->phys_dev, queueFamilyIndex, surface, pSupported);
}

static VkResult WINAPI wine_vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t *pDataSize, void *pData)
{
    TRACE("%p, 0x%s, %p, %p\n", device, wine_dbgstr_longlong(pipelineCache), pDataSize, pData);
    return device->funcs.p_vkGetPipelineCacheData(device->device, pipelineCache, pDataSize, pData);
}

static VkResult WINAPI wine_vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void *pData, VkDeviceSize stride, VkQueryResultFlags flags)
{
    TRACE("%p, 0x%s, %u, %u, 0x%s, %p, 0x%s, %#x\n", device, wine_dbgstr_longlong(queryPool), firstQuery, queryCount, wine_dbgstr_longlong(dataSize), pData, wine_dbgstr_longlong(stride), flags);
    return device->funcs.p_vkGetQueryPoolResults(device->device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
}

static void WINAPI wine_vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D *pGranularity)
{
    TRACE("%p, 0x%s, %p\n", device, wine_dbgstr_longlong(renderPass), pGranularity);
    device->funcs.p_vkGetRenderAreaGranularity(device->device, renderPass, pGranularity);
}

static VkResult WINAPI wine_vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages)
{
    TRACE("%p, 0x%s, %p, %p\n", device, wine_dbgstr_longlong(swapchain), pSwapchainImageCount, pSwapchainImages);
    return device->funcs.p_vkGetSwapchainImagesKHR(device->device, swapchain, pSwapchainImageCount, pSwapchainImages);
}

static VkResult WINAPI wine_vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange *pMemoryRanges)
{
    TRACE("%p, %u, %p\n", device, memoryRangeCount, pMemoryRanges);
    return device->funcs.p_vkInvalidateMappedMemoryRanges(device->device, memoryRangeCount, pMemoryRanges);
}

static VkResult WINAPI wine_vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **ppData)
{
    TRACE("%p, 0x%s, 0x%s, 0x%s, %#x, %p\n", device, wine_dbgstr_longlong(memory), wine_dbgstr_longlong(offset), wine_dbgstr_longlong(size), flags, ppData);
    return device->funcs.p_vkMapMemory(device->device, memory, offset, size, flags, ppData);
}

static VkResult WINAPI wine_vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache *pSrcCaches)
{
    TRACE("%p, 0x%s, %u, %p\n", device, wine_dbgstr_longlong(dstCache), srcCacheCount, pSrcCaches);
    return device->funcs.p_vkMergePipelineCaches(device->device, dstCache, srcCacheCount, pSrcCaches);
}

static VkResult WINAPI wine_vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo *pBindInfo, VkFence fence)
{
    FIXME("stub: %p, %u, %p, 0x%s\n", queue, bindInfoCount, pBindInfo, wine_dbgstr_longlong(fence));
    return VK_ERROR_OUT_OF_HOST_MEMORY;
}

static VkResult WINAPI wine_vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR *pPresentInfo)
{
    FIXME("stub: %p, %p\n", queue, pPresentInfo);
    return VK_ERROR_OUT_OF_HOST_MEMORY;
}

static VkResult WINAPI wine_vkQueueWaitIdle(VkQueue queue)
{
    FIXME("stub: %p\n", queue);
    return VK_ERROR_OUT_OF_HOST_MEMORY;
}

static VkResult WINAPI wine_vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags)
{
    TRACE("%p, %#x\n", commandBuffer, flags);
    return commandBuffer->device->funcs.p_vkResetCommandBuffer(commandBuffer->command_buffer, flags);
}

static VkResult WINAPI wine_vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags)
{
    TRACE("%p, 0x%s, %#x\n", device, wine_dbgstr_longlong(commandPool), flags);
    return device->funcs.p_vkResetCommandPool(device->device, commandPool, flags);
}

static VkResult WINAPI wine_vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags)
{
    TRACE("%p, 0x%s, %#x\n", device, wine_dbgstr_longlong(descriptorPool), flags);
    return device->funcs.p_vkResetDescriptorPool(device->device, descriptorPool, flags);
}

static VkResult WINAPI wine_vkResetEvent(VkDevice device, VkEvent event)
{
    TRACE("%p, 0x%s\n", device, wine_dbgstr_longlong(event));
    return device->funcs.p_vkResetEvent(device->device, event);
}

static VkResult WINAPI wine_vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences)
{
    TRACE("%p, %u, %p\n", device, fenceCount, pFences);
    return device->funcs.p_vkResetFences(device->device, fenceCount, pFences);
}

static VkResult WINAPI wine_vkSetEvent(VkDevice device, VkEvent event)
{
    TRACE("%p, 0x%s\n", device, wine_dbgstr_longlong(event));
    return device->funcs.p_vkSetEvent(device->device, event);
}

static void WINAPI wine_vkUnmapMemory(VkDevice device, VkDeviceMemory memory)
{
    TRACE("%p, 0x%s\n", device, wine_dbgstr_longlong(memory));
    device->funcs.p_vkUnmapMemory(device->device, memory);
}

static void WINAPI wine_vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet *pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet *pDescriptorCopies)
{
    TRACE("%p, %u, %p, %u, %p\n", device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    device->funcs.p_vkUpdateDescriptorSets(device->device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

static VkResult WINAPI wine_vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences, VkBool32 waitAll, uint64_t timeout)
{
    TRACE("%p, %u, %p, %u, 0x%s\n", device, fenceCount, pFences, waitAll, wine_dbgstr_longlong(timeout));
    return device->funcs.p_vkWaitForFences(device->device, fenceCount, pFences, waitAll, timeout);
}

static const struct vulkan_func vk_device_dispatch_table[] = {
    {"vkAcquireNextImageKHR", &wine_vkAcquireNextImageKHR},
    {"vkAllocateCommandBuffers", &wine_vkAllocateCommandBuffers},
    {"vkAllocateDescriptorSets", &wine_vkAllocateDescriptorSets},
    {"vkAllocateMemory", &wine_vkAllocateMemory},
    {"vkBeginCommandBuffer", &wine_vkBeginCommandBuffer},
    {"vkBindBufferMemory", &wine_vkBindBufferMemory},
    {"vkBindImageMemory", &wine_vkBindImageMemory},
    {"vkCmdBeginQuery", &wine_vkCmdBeginQuery},
    {"vkCmdBeginRenderPass", &wine_vkCmdBeginRenderPass},
    {"vkCmdBindDescriptorSets", &wine_vkCmdBindDescriptorSets},
    {"vkCmdBindIndexBuffer", &wine_vkCmdBindIndexBuffer},
    {"vkCmdBindPipeline", &wine_vkCmdBindPipeline},
    {"vkCmdBindVertexBuffers", &wine_vkCmdBindVertexBuffers},
    {"vkCmdBlitImage", &wine_vkCmdBlitImage},
    {"vkCmdClearAttachments", &wine_vkCmdClearAttachments},
    {"vkCmdClearColorImage", &wine_vkCmdClearColorImage},
    {"vkCmdClearDepthStencilImage", &wine_vkCmdClearDepthStencilImage},
    {"vkCmdCopyBuffer", &wine_vkCmdCopyBuffer},
    {"vkCmdCopyBufferToImage", &wine_vkCmdCopyBufferToImage},
    {"vkCmdCopyImage", &wine_vkCmdCopyImage},
    {"vkCmdCopyImageToBuffer", &wine_vkCmdCopyImageToBuffer},
    {"vkCmdCopyQueryPoolResults", &wine_vkCmdCopyQueryPoolResults},
    {"vkCmdDispatch", &wine_vkCmdDispatch},
    {"vkCmdDispatchIndirect", &wine_vkCmdDispatchIndirect},
    {"vkCmdDraw", &wine_vkCmdDraw},
    {"vkCmdDrawIndexed", &wine_vkCmdDrawIndexed},
    {"vkCmdDrawIndexedIndirect", &wine_vkCmdDrawIndexedIndirect},
    {"vkCmdDrawIndirect", &wine_vkCmdDrawIndirect},
    {"vkCmdEndQuery", &wine_vkCmdEndQuery},
    {"vkCmdEndRenderPass", &wine_vkCmdEndRenderPass},
    {"vkCmdExecuteCommands", &wine_vkCmdExecuteCommands},
    {"vkCmdFillBuffer", &wine_vkCmdFillBuffer},
    {"vkCmdNextSubpass", &wine_vkCmdNextSubpass},
    {"vkCmdPipelineBarrier", &wine_vkCmdPipelineBarrier},
    {"vkCmdPushConstants", &wine_vkCmdPushConstants},
    {"vkCmdResetEvent", &wine_vkCmdResetEvent},
    {"vkCmdResetQueryPool", &wine_vkCmdResetQueryPool},
    {"vkCmdResolveImage", &wine_vkCmdResolveImage},
    {"vkCmdSetBlendConstants", &wine_vkCmdSetBlendConstants},
    {"vkCmdSetDepthBias", &wine_vkCmdSetDepthBias},
    {"vkCmdSetDepthBounds", &wine_vkCmdSetDepthBounds},
    {"vkCmdSetEvent", &wine_vkCmdSetEvent},
    {"vkCmdSetLineWidth", &wine_vkCmdSetLineWidth},
    {"vkCmdSetScissor", &wine_vkCmdSetScissor},
    {"vkCmdSetStencilCompareMask", &wine_vkCmdSetStencilCompareMask},
    {"vkCmdSetStencilReference", &wine_vkCmdSetStencilReference},
    {"vkCmdSetStencilWriteMask", &wine_vkCmdSetStencilWriteMask},
    {"vkCmdSetViewport", &wine_vkCmdSetViewport},
    {"vkCmdUpdateBuffer", &wine_vkCmdUpdateBuffer},
    {"vkCmdWaitEvents", &wine_vkCmdWaitEvents},
    {"vkCmdWriteTimestamp", &wine_vkCmdWriteTimestamp},
    {"vkCreateBuffer", &wine_vkCreateBuffer},
    {"vkCreateBufferView", &wine_vkCreateBufferView},
    {"vkCreateCommandPool", &wine_vkCreateCommandPool},
    {"vkCreateComputePipelines", &wine_vkCreateComputePipelines},
    {"vkCreateDescriptorPool", &wine_vkCreateDescriptorPool},
    {"vkCreateDescriptorSetLayout", &wine_vkCreateDescriptorSetLayout},
    {"vkCreateEvent", &wine_vkCreateEvent},
    {"vkCreateFence", &wine_vkCreateFence},
    {"vkCreateFramebuffer", &wine_vkCreateFramebuffer},
    {"vkCreateGraphicsPipelines", &wine_vkCreateGraphicsPipelines},
    {"vkCreateImage", &wine_vkCreateImage},
    {"vkCreateImageView", &wine_vkCreateImageView},
    {"vkCreatePipelineCache", &wine_vkCreatePipelineCache},
    {"vkCreatePipelineLayout", &wine_vkCreatePipelineLayout},
    {"vkCreateQueryPool", &wine_vkCreateQueryPool},
    {"vkCreateRenderPass", &wine_vkCreateRenderPass},
    {"vkCreateSampler", &wine_vkCreateSampler},
    {"vkCreateSemaphore", &wine_vkCreateSemaphore},
    {"vkCreateShaderModule", &wine_vkCreateShaderModule},
    {"vkCreateSwapchainKHR", &wine_vkCreateSwapchainKHR},
    {"vkDestroyBuffer", &wine_vkDestroyBuffer},
    {"vkDestroyBufferView", &wine_vkDestroyBufferView},
    {"vkDestroyCommandPool", &wine_vkDestroyCommandPool},
    {"vkDestroyDescriptorPool", &wine_vkDestroyDescriptorPool},
    {"vkDestroyDescriptorSetLayout", &wine_vkDestroyDescriptorSetLayout},
    {"vkDestroyDevice", &wine_vkDestroyDevice},
    {"vkDestroyEvent", &wine_vkDestroyEvent},
    {"vkDestroyFence", &wine_vkDestroyFence},
    {"vkDestroyFramebuffer", &wine_vkDestroyFramebuffer},
    {"vkDestroyImage", &wine_vkDestroyImage},
    {"vkDestroyImageView", &wine_vkDestroyImageView},
    {"vkDestroyPipeline", &wine_vkDestroyPipeline},
    {"vkDestroyPipelineCache", &wine_vkDestroyPipelineCache},
    {"vkDestroyPipelineLayout", &wine_vkDestroyPipelineLayout},
    {"vkDestroyQueryPool", &wine_vkDestroyQueryPool},
    {"vkDestroyRenderPass", &wine_vkDestroyRenderPass},
    {"vkDestroySampler", &wine_vkDestroySampler},
    {"vkDestroySemaphore", &wine_vkDestroySemaphore},
    {"vkDestroyShaderModule", &wine_vkDestroyShaderModule},
    {"vkDestroySwapchainKHR", &wine_vkDestroySwapchainKHR},
    {"vkDeviceWaitIdle", &wine_vkDeviceWaitIdle},
    {"vkEndCommandBuffer", &wine_vkEndCommandBuffer},
    {"vkFlushMappedMemoryRanges", &wine_vkFlushMappedMemoryRanges},
    {"vkFreeCommandBuffers", &wine_vkFreeCommandBuffers},
    {"vkFreeDescriptorSets", &wine_vkFreeDescriptorSets},
    {"vkFreeMemory", &wine_vkFreeMemory},
    {"vkGetBufferMemoryRequirements", &wine_vkGetBufferMemoryRequirements},
    {"vkGetDeviceMemoryCommitment", &wine_vkGetDeviceMemoryCommitment},
    {"vkGetDeviceProcAddr", &wine_vkGetDeviceProcAddr},
    {"vkGetDeviceQueue", &wine_vkGetDeviceQueue},
    {"vkGetEventStatus", &wine_vkGetEventStatus},
    {"vkGetFenceStatus", &wine_vkGetFenceStatus},
    {"vkGetImageMemoryRequirements", &wine_vkGetImageMemoryRequirements},
    {"vkGetImageSparseMemoryRequirements", &wine_vkGetImageSparseMemoryRequirements},
    {"vkGetImageSubresourceLayout", &wine_vkGetImageSubresourceLayout},
    {"vkGetPipelineCacheData", &wine_vkGetPipelineCacheData},
    {"vkGetQueryPoolResults", &wine_vkGetQueryPoolResults},
    {"vkGetRenderAreaGranularity", &wine_vkGetRenderAreaGranularity},
    {"vkGetSwapchainImagesKHR", &wine_vkGetSwapchainImagesKHR},
    {"vkInvalidateMappedMemoryRanges", &wine_vkInvalidateMappedMemoryRanges},
    {"vkMapMemory", &wine_vkMapMemory},
    {"vkMergePipelineCaches", &wine_vkMergePipelineCaches},
    {"vkQueueBindSparse", &wine_vkQueueBindSparse},
    {"vkQueuePresentKHR", &wine_vkQueuePresentKHR},
    {"vkQueueSubmit", &wine_vkQueueSubmit},
    {"vkQueueWaitIdle", &wine_vkQueueWaitIdle},
    {"vkResetCommandBuffer", &wine_vkResetCommandBuffer},
    {"vkResetCommandPool", &wine_vkResetCommandPool},
    {"vkResetDescriptorPool", &wine_vkResetDescriptorPool},
    {"vkResetEvent", &wine_vkResetEvent},
    {"vkResetFences", &wine_vkResetFences},
    {"vkSetEvent", &wine_vkSetEvent},
    {"vkUnmapMemory", &wine_vkUnmapMemory},
    {"vkUpdateDescriptorSets", &wine_vkUpdateDescriptorSets},
    {"vkWaitForFences", &wine_vkWaitForFences},
};

static const struct vulkan_func vk_instance_dispatch_table[] = {
    {"vkCreateDevice", &wine_vkCreateDevice},
    {"vkCreateWin32SurfaceKHR", &wine_vkCreateWin32SurfaceKHR},
    {"vkDestroyInstance", &wine_vkDestroyInstance},
    {"vkDestroySurfaceKHR", &wine_vkDestroySurfaceKHR},
    {"vkEnumerateDeviceExtensionProperties", &wine_vkEnumerateDeviceExtensionProperties},
    {"vkEnumerateDeviceLayerProperties", &wine_vkEnumerateDeviceLayerProperties},
    {"vkEnumeratePhysicalDevices", &wine_vkEnumeratePhysicalDevices},
    {"vkGetPhysicalDeviceFeatures", &wine_vkGetPhysicalDeviceFeatures},
    {"vkGetPhysicalDeviceFormatProperties", &wine_vkGetPhysicalDeviceFormatProperties},
    {"vkGetPhysicalDeviceImageFormatProperties", &wine_vkGetPhysicalDeviceImageFormatProperties},
    {"vkGetPhysicalDeviceMemoryProperties", &wine_vkGetPhysicalDeviceMemoryProperties},
    {"vkGetPhysicalDeviceProperties", &wine_vkGetPhysicalDeviceProperties},
    {"vkGetPhysicalDeviceQueueFamilyProperties", &wine_vkGetPhysicalDeviceQueueFamilyProperties},
    {"vkGetPhysicalDeviceSparseImageFormatProperties", &wine_vkGetPhysicalDeviceSparseImageFormatProperties},
    {"vkGetPhysicalDeviceSurfaceCapabilitiesKHR", &wine_vkGetPhysicalDeviceSurfaceCapabilitiesKHR},
    {"vkGetPhysicalDeviceSurfaceFormatsKHR", &wine_vkGetPhysicalDeviceSurfaceFormatsKHR},
    {"vkGetPhysicalDeviceSurfacePresentModesKHR", &wine_vkGetPhysicalDeviceSurfacePresentModesKHR},
    {"vkGetPhysicalDeviceSurfaceSupportKHR", &wine_vkGetPhysicalDeviceSurfaceSupportKHR},
    {"vkGetPhysicalDeviceWin32PresentationSupportKHR", &wine_vkGetPhysicalDeviceWin32PresentationSupportKHR},
};
void *wine_vk_get_device_proc_addr(const char *name)
{
    int i;
    for (i = 0; i < sizeof(vk_device_dispatch_table) / sizeof(vk_device_dispatch_table[0]); i++)
    {
        if (strcmp(name, vk_device_dispatch_table[i].name) == 0)
        {
            TRACE("Found pName=%s in device table\n", name);
            return vk_device_dispatch_table[i].func;
        }
    }
    return NULL;
}

void *wine_vk_get_instance_proc_addr(const char *name)
{
    int i;
    for (i = 0; i < sizeof(vk_instance_dispatch_table) / sizeof(vk_instance_dispatch_table[0]); i++)
    {
        if (strcmp(name, vk_instance_dispatch_table[i].name) == 0)
        {
            TRACE("Found pName=%s in instance table\n", name);
            return vk_instance_dispatch_table[i].func;
        }
    }
    return NULL;
}
