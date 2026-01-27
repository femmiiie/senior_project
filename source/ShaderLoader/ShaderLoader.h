#include <webgpu/webgpu.hpp>

#include <fstream>
#include <sstream>

#include <filesystem>

wgpu::ShaderModule LoadShader(wgpu::Device &device, std::string filepath)
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

  wgpu::ShaderModuleDescriptor shaderDesc;

#ifdef WEBGPU_BACKEND_EMSCRIPTEN // need to come up with something for this
  wgpu::ShaderModuleWGSLDescriptor wgslDesc;
  wgslDesc.chain.next = nullptr;
  wgslDesc.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
  wgslDesc.code = shaderSource.c_str();
  shaderDesc.nextInChain = &wgslDesc.chain;
#else
  wgpu::ShaderSourceWGSL shaderCodeDesc;

  shaderCodeDesc.chain.next = nullptr;
  shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;

  shaderDesc.nextInChain = &shaderCodeDesc.chain;
  shaderCodeDesc.code.data = shaderSource.c_str();
  shaderCodeDesc.code.length = shaderSource.length();
#endif

  return device.createShaderModule(shaderDesc);
}