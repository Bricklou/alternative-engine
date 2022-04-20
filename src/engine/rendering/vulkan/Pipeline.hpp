#pragma once

#include "Device.hpp"

namespace AltE::Rendering {

  struct PipelineConfigInfo {
      PipelineConfigInfo() = default;
      PipelineConfigInfo(const PipelineConfigInfo &) = delete;
      PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

      std::vector<VkVertexInputBindingDescription> bindingDescription{};
      std::vector<VkVertexInputAttributeDescription> attributeDescription{};
      VkPipelineViewportStateCreateInfo viewportInfo;
      VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
      VkPipelineRasterizationStateCreateInfo rasterizationInfo;
      VkPipelineMultisampleStateCreateInfo multisampleInfo;
      VkPipelineColorBlendAttachmentState colorBlendAttachment;
      VkPipelineColorBlendStateCreateInfo colorBlendInfo;
      VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
      std::vector<VkDynamicState> dynamicStateEnables;
      VkPipelineDynamicStateCreateInfo dynamicStateInfo;
      VkPipelineLayout pipelineLayout = nullptr;
      VkRenderPass renderPass = nullptr;
      uint32_t subpass = 0;
  };

  class Pipeline {
    public:
      Pipeline();
      ~Pipeline();

    private:
      Device &device;
      VkPipeline graphicsPipeline;

      VkShaderModule vertShaderModule;
      VkShaderModule fragShaderModule;
  };
} // namespace AltE::Rendering