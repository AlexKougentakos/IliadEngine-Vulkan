﻿#include "Descriptors.h"

#include <cassert>
#include <stdexcept>

namespace ili
{

    // *************** Descriptor Set Layout Builder *********************

    DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) {
        assert(!m_Bindings.contains(binding) && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        m_Bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const 
    {
        return std::make_unique<DescriptorSetLayout>(m_Device, m_Bindings);
    }

    // *************** Descriptor Set Layout *********************

    DescriptorSetLayout::DescriptorSetLayout(
        Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : m_Device{ device }, m_Bindings{ bindings } 
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            device.GetDevice(),
            &descriptorSetLayoutInfo,
            nullptr,
            &m_DescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    DescriptorSetLayout::~DescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(m_Device.GetDevice(), m_DescriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count) {
        m_PoolSizes.push_back({ descriptorType, count });
        return *this;
    }

    DescriptorPool::Builder& DescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags) {
        m_PoolFlags = flags;
        return *this;
    }
    DescriptorPool::Builder& DescriptorPool::Builder::setMaxSets(uint32_t count) {
        m_MaxSets = count;
        return *this;
    }

    std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
        return std::make_unique<DescriptorPool>(m_Device, m_MaxSets, m_PoolFlags, m_PoolSizes);
    }

    // *************** Descriptor Pool *********************

    DescriptorPool::DescriptorPool(
        Device& device,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes)
        : m_Device{ device } 
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(device.GetDevice(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    DescriptorPool::~DescriptorPool() 
    {
        vkDestroyDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, nullptr);
    }

    bool DescriptorPool::AllocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const 
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(m_Device.GetDevice(), &allocInfo, &descriptor) != VK_SUCCESS) 
        {
            return false;
        }
        return true;
    }

    void DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const 
    {
        vkFreeDescriptorSets(
            m_Device.GetDevice(),
            m_DescriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void DescriptorPool::ResetPool() 
    {
        vkResetDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool)
        : m_SetLayout{ setLayout }, m_Pool{ pool } {}

    DescriptorWriter& DescriptorWriter::WriteBuffer(
        uint32_t binding, VkDescriptorBufferInfo* bufferInfo) 
    {
        assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

        const auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    DescriptorWriter& DescriptorWriter::WriteImage(
        uint32_t binding, VkDescriptorImageInfo* imageInfo) 
    {
        assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

        const auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    bool DescriptorWriter::Build(VkDescriptorSet& set) 
    {
        const bool success = m_Pool.AllocateDescriptor(m_SetLayout.GetDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        Overwrite(set);
        return true;
    }

    void DescriptorWriter::Overwrite(VkDescriptorSet& set) 
    {
        for (auto& write : m_Writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_Pool.m_Device.GetDevice(), m_Writes.size(), m_Writes.data(), 0, nullptr);
    }

}