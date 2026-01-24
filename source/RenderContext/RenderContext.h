#ifndef RENDERCONTEXT
#define RENDERCONTEXT

#include <webgpu/webgpu.hpp>

struct RenderContext
{
  wgpu::Instance instance;
  wgpu::Device device;
  wgpu::Queue queue;
  wgpu::Surface surface;
  wgpu::TextureFormat surfaceFormat;

  bool ok = true;

  virtual wgpu::TextureView GetNextTextureView() = 0;
  virtual void Present() = 0;
  virtual void DevicePoll() = 0;

  virtual ~RenderContext() = default;
};

#endif