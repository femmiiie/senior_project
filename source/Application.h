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

private:
  void InitializePipeline();

private:
  RenderContext &context;
  wgpu::RenderPipeline pipeline;
};

#endif