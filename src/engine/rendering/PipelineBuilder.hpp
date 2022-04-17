#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace AltE {
  class PipelineBuilder {
    public:
      std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
      VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
      VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
      VkViewport _viewport;
      VkRect2D _scissor;
      VkPipelineRasterizationStateCreateInfo _rasterizer;
      VkPipelineColorBlendAttachmentState _colorBlendAttachment;
      VkPipelineMultisampleStateCreateInfo _multisampling;
      VkPipelineLayout _pipelineLayout;

      VkPipeline build_pipeline(const VkDevice &device,
                                const VkRenderPass &renderPass);
  };
} // namespace AltE