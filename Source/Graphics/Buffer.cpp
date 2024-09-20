/*
 * Encapsulates a vulkan m_Buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */

#include "Buffer.h"

#include <cassert>
#include <cstring>

namespace ili
{

    /**
     * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
     *
     * @param instanceSize The size of an instance
     * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
     * minUniformBufferOffsetAlignment)
     *
     * @return VkResult of the buffer mapping call
     */
    VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) 
    {
        if (minOffsetAlignment > 0) {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    Buffer::Buffer(
        Device& device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment)
        : m_Device{ device },
        m_InstanceSize{ instanceSize },
        m_InstanceCount{ instanceCount },
        m_UsageFlags{ usageFlags },
        m_MemoryPropertyFlags{ memoryPropertyFlags } 
    {
        m_AlignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
        m_BufferSize = m_AlignmentSize * instanceCount;
        device.CreateBuffer(m_BufferSize, usageFlags, memoryPropertyFlags, m_Buffer, m_Memory);
    }

    Buffer::~Buffer() 
    {
        Unmap();
        vkDestroyBuffer(m_Device.GetDevice(), m_Buffer, nullptr);
        vkFreeMemory(m_Device.GetDevice(), m_Memory, nullptr);
    }

    /**
     * Map a m_Memory range of this m_Buffer. If successful, m_Mapped points to the specified m_Buffer range.
     *
     * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
     * buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the buffer mapping call
     */
    VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset) 
    {
        assert(m_Buffer && m_Memory && "Called Map on m_Buffer before create");
        return vkMapMemory(m_Device.GetDevice(), m_Memory, offset, size, 0, &m_Mapped);
    }

    /**
     * Unmap a m_Mapped m_Memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void Buffer::Unmap() 
    {
        if (m_Mapped) 
        {
            vkUnmapMemory(m_Device.GetDevice(), m_Memory);
            m_Mapped = nullptr;
        }
    }

    /**
     * Copies the specified data to the m_Mapped m_Buffer. Default value writes whole m_Buffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
     * range.
     * @param offset (Optional) Byte offset from beginning of mapped region
     *
     */
    void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
    {
        assert(m_Mapped && "Cannot copy to unmapped m_Buffer");

        if (size == VK_WHOLE_SIZE) 
        {
            memcpy(m_Mapped, data, m_BufferSize);
        }
        else 
        {
            char* memOffset = (char*)m_Mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    /**
     * Flush a m_Memory range of the m_Buffer to make it visible to the device
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
     * complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the flush call
     */
    VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset) 
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_Memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(m_Device.GetDevice(), 1, &mappedRange);
    }

    /**
     * Invalidate a m_Memory range of the m_Buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
     * the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the invalidate call
     */
    VkResult Buffer::Invalidate(VkDeviceSize size, VkDeviceSize offset) 
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_Memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(m_Device.GetDevice(), 1, &mappedRange);
    }

    /**
     * Create a m_Buffer info descriptor
     *
     * @param size (Optional) Size of the memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset) 
    {
        return VkDescriptorBufferInfo{
            m_Buffer,
            offset,
            size,
        };
    }

    /**
     * Copies "m_InstanceSize" bytes of data to the m_Mapped m_Buffer at an offset of index * m_AlignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset calculation
     *
     */
    void Buffer::WriteToIndex(void* data, int index)
    {
        WriteToBuffer(data, m_InstanceSize, index * m_AlignmentSize);
    }

    /**
     *  Flush the m_Memory range at index * m_AlignmentSize of the m_Buffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    VkResult Buffer::FlushIndex(int index) { return Flush(m_AlignmentSize, index * m_AlignmentSize); }

    /**
     * Create a m_Buffer info descriptor
     *
     * @param index Specifies the region given by index * alignmentSize
     *
     * @return VkDescriptorBufferInfo for instance at index
     */
    VkDescriptorBufferInfo Buffer::DescriptorInfoForIndex(int index) 
    {
        return DescriptorInfo(m_AlignmentSize, index * m_AlignmentSize);
    }

    /**
     * Invalidate a m_Memory range of the m_Buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param index Specifies the region to invalidate: index * alignmentSize
     *
     * @return VkResult of the invalidate call
     */
    VkResult Buffer::InvalidateIndex(int index) 
    {
        return Invalidate(m_AlignmentSize, index * m_AlignmentSize);
    }

}