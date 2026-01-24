#include <webgpu/webgpu.hpp>

#include <fstream>
#include <sstream>

#include <filesystem>

wgpu::ShaderModule LoadShader(wgpu::Device& device, std::string filepath)
{
  std::ifstream fs(filepath);

  if (!fs.is_open())
  {
    std::cout << "Failed to open file " << filepath << std::endl;
    return nullptr;
  }

  std::ostringstream ss;
  ss << fs.rdbuf();
  std::string shaderSource = ss.str();

  std::cout << shaderSource << std::endl;

  // Load the shader module
  wgpu::ShaderModuleDescriptor shaderDesc;

  // We use the extension mechanism to specify the WGSL part of the shader module descriptor
  wgpu::ShaderSourceWGSL shaderCodeDesc;
  // Set the chained struct's header
  shaderCodeDesc.chain.next = nullptr;
  shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
  // Connect the chain
  shaderDesc.nextInChain = &shaderCodeDesc.chain;
  shaderCodeDesc.code.data = shaderSource.c_str();
  shaderCodeDesc.code.length = shaderSource.length();
  
  return device.createShaderModule(shaderDesc);
}