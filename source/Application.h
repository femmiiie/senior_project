#ifndef APPLICATION
#define APPLICATION

#include <webgpu/webgpu.hpp>

#include <iostream>
#include <cassert>
#include <vector>
#include <cstring>

#include "RenderContext/RenderContext.h"

class Application
{
public:
  Application(RenderContext &context);
  ~Application();
  void MainLoop();
  bool IsRunning();

private:
  wgpu::TextureView GetNextSurfaceTextureView();

  // Substep of Initialize() that creates the render pipeline
  void InitializePipeline();

private:
  RenderContext &context;
  wgpu::RenderPipeline pipeline;
};

#endif