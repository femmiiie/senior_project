#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>

#include <emscripten.h>

#include <iostream>
#include <cassert>
#include <vector>
#include <cstring>

bool init()
{
  wgpu::InstanceDescriptor instanceDesc = {};
  wgpu::Instance instance = wgpuCreateInstance(&instanceDesc);
  if (!instance)
  {
    std::cerr << "Failed to create WebGPU instance!" << std::endl;
    return false;
  }
  std::cout << "Created instance: " << instance << std::endl;

  WGPUSurface rawSurface = emscripten_webgpu_get_surface(instance);
  if (!rawSurface)
  {
    std::cerr << "Failed to create surface from canvas!" << std::endl;
    return false;
  }
  surface = Surface(rawSurface);
  std::cout << "Created surface: " << surface << std::endl;

  std::cout << "Requesting adapter..." << std::endl;
  RequestAdapterOptions adapterOpts = {};
  adapterOpts.compatibleSurface = surface;
  Adapter adapter = instance.requestAdapter(adapterOpts);
  std::cout << "Got adapter: " << adapter << std::endl;

  std::cout << "Requesting device..." << std::endl;
  DeviceDescriptor deviceDesc = {};
  deviceDesc.label = WGPU_STRING_VIEW_INIT;
  deviceDesc.requiredFeatureCount = 0;
  deviceDesc.requiredLimits = nullptr;
  deviceDesc.defaultQueue.nextInChain = nullptr;
  deviceDesc.defaultQueue.label = WGPU_STRING_VIEW_INIT;

  device = adapter.requestDevice(deviceDesc);
  std::cout << "Got device: " << device << std::endl;

  queue = device.getQueue();

  // Configure the surface
  SurfaceConfiguration config = {};

  // Configuration of the textures created for the underlying swap chain
  config.width = 640;
  config.height = 480;
  config.usage = TextureUsage::RenderAttachment;
  SurfaceCapabilities capabilities;
  surface.getCapabilities(adapter, &capabilities);
  surfaceFormat = capabilities.formats[0];
  config.format = surfaceFormat;

  // And we do not need any particular view format:
  config.viewFormatCount = 0;
  config.viewFormats = nullptr;
  config.device = device;
  config.presentMode = PresentMode::Fifo;
  config.alphaMode = CompositeAlphaMode::Auto;

  surface.configure(config);

  InitializePipeline();

  return true;
}

int main()
{
  Application app;

  if (!app.Initialize())
  {
    return 1;
  }

  auto callback = [](void *arg)
  {
    Application *pApp = reinterpret_cast<Application *>(arg);
    pApp->MainLoop(); // 4. We can use the application object
  };
  emscripten_set_main_loop_arg(callback, &app, 0, true);

  app.Terminate();

  return 0;
}