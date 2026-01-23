#ifndef RENDERCONTEXT
#define RENDERCONTEXT

#include <webgpu/webgpu.hpp>

struct RenderContext
{
  wgpu::Instance instance;
  wgpu::Device device;
  wgpu::Queue queue;
  wgpu::TextureFormat surfaceFormat;

  bool ok = true;

  virtual bool isRunning() = 0;

  // virtual wgpu::TextureView AcquireFrame() = 0;
  // virtual void Present() = 0;
  // virtual uint32_t Width() const = 0;
  // virtual uint32_t Height() const = 0;

  virtual ~RenderContext() = default;
};

#endif