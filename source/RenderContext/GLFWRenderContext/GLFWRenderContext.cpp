#include "GLFWRenderContext.h"

#include <iostream>

GLFWRenderContext::GLFWRenderContext()
{
  if (!glfwInit())
  {
    std::cerr << "Failed to initialize GLFW!" << std::endl;
    this->ok = false;
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  this->window = glfwCreateWindow(640, 480, "iPASS for WebGPU", nullptr, nullptr);
  if (!this->window)
  {
    std::cerr << "Failed to create GLFW window!" << std::endl;
    glfwTerminate();
    this->ok = false;
    return;
  }

  wgpu::InstanceDescriptor instanceDesc = {};
  this->instance = wgpuCreateInstance(&instanceDesc);
  if (!this->instance)
  {
    std::cerr << "Failed to create WebGPU instance!" << std::endl;
    this->ok = false;
    return;
  }
  std::cout << "Created instance: " << this->instance << std::endl;

  WGPUSurface rawSurface = glfwCreateWindowWGPUSurface(this->instance, this->window);
  if (!rawSurface)
  {
    std::cerr << "Failed to create surface!" << std::endl;
    this->ok = false;
    return;
  }
  this->surface = wgpu::Surface(rawSurface);
  std::cout << "Created surface: " << this->surface << std::endl;

  std::cout << "Requesting adapter..." << std::endl;
  wgpu::RequestAdapterOptions adapterOpts = {};
  adapterOpts.compatibleSurface = this->surface;
  wgpu::Adapter adapter = this->instance.requestAdapter(adapterOpts);
  std::cout << "Got adapter: " << adapter << std::endl;

  std::cout << "Requesting device..." << std::endl;
  wgpu::DeviceDescriptor deviceDesc = {};
  deviceDesc.label = WGPU_STRING_VIEW_INIT;
  deviceDesc.requiredFeatureCount = 0;
  deviceDesc.requiredLimits = nullptr;
  deviceDesc.defaultQueue.nextInChain = nullptr;
  deviceDesc.defaultQueue.label = WGPU_STRING_VIEW_INIT;

  this->device = adapter.requestDevice(deviceDesc);
  std::cout << "Got device: " << this->device << std::endl;

  queue = device.getQueue();

  // Configure the surface
  wgpu::SurfaceConfiguration config = {};

  // Configuration of the textures created for the underlying swap chain
  config.width = 640;
  config.height = 480;
  config.usage = wgpu::TextureUsage::RenderAttachment;
  wgpu::SurfaceCapabilities capabilities;
  this->surface.getCapabilities(adapter, &capabilities);
  this->surfaceFormat = capabilities.formats[0];
  config.format = this->surfaceFormat;

  // And we do not need any particular view format:
  config.viewFormatCount = 0;
  config.viewFormats = nullptr;
  config.device = device;
  config.presentMode = wgpu::PresentMode::Fifo;
  config.alphaMode = wgpu::CompositeAlphaMode::Auto;

  this->surface.configure(config);

  adapter.release();
  this->instance.release();
}

GLFWRenderContext::~GLFWRenderContext()
{
  this->surface.unconfigure();
  queue.release();
  this->surface.release();
  device.release();
  glfwDestroyWindow(this->window);
  glfwTerminate();
}

bool GLFWRenderContext::isRunning()
{
  return !glfwWindowShouldClose(this->window);
}

wgpu::TextureView GLFWRenderContext::GetNextTextureView()
{
  wgpu::SurfaceTexture surfaceTexture;
  this->surface.getCurrentTexture(&surfaceTexture);
  if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal)
  {
    return nullptr;
  }
  wgpu::Texture texture = surfaceTexture.texture;

  wgpu::TextureViewDescriptor viewDescriptor;
  viewDescriptor.label = WGPU_STRING_VIEW_INIT;
  viewDescriptor.format = texture.getFormat();
  viewDescriptor.dimension = wgpu::TextureViewDimension::_2D;
  viewDescriptor.baseMipLevel = 0;
  viewDescriptor.mipLevelCount = 1;
  viewDescriptor.baseArrayLayer = 0;
  viewDescriptor.arrayLayerCount = 1;
  viewDescriptor.aspect = wgpu::TextureAspect::All;
  wgpu::TextureView targetView = texture.createView(viewDescriptor);

#ifndef WEBGPU_BACKEND_WGPU
  wgpu::Texture(surfaceTexture.texture).release();
#endif

  return targetView;
}

void GLFWRenderContext::Present()
{
  this->surface.present();
}

void GLFWRenderContext::DevicePoll()
{
#if defined(WEBGPU_BACKEND_DAWN)
  this->device.tick();
#elif defined(WEBGPU_BACKEND_WGPU)
  this->device.poll(false, nullptr);
#endif
}