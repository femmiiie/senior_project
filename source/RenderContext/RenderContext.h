#ifndef RENDERCONTEXT
#define RENDERCONTEXT

#include <exception>

#include <webgpu/webgpu.hpp>

struct RenderContext
{
  wgpu::Adapter adapter;
  wgpu::Instance instance;
  wgpu::Device device;
  wgpu::Queue queue;
  wgpu::Surface surface;
  wgpu::TextureFormat surfaceFormat;

  void Initialize();
  void ConfigureSurface();

  virtual void GenerateSurface() = 0;
  virtual void GetSurfaceFormat() = 0;

  virtual wgpu::TextureView GetNextTextureView() = 0;
  virtual void Present() = 0;
  virtual void DevicePoll() = 0;

  ~RenderContext();
};

class RenderContextException : public std::exception
{
private:
  std::string message;

public:
  RenderContextException(std::string m) : message(m) {}
  RenderContextException(const char m) { this->message = m; }
  const char *what() const noexcept override { return this->message.c_str(); }
};

#endif