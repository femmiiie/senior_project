#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>

#include <iostream>
// #include <cassert>
// #include <vector>
// #include <cstring>

#include "Application.h"
#include "RenderContext/GLFWRenderContext/GLFWRenderContext.h"

int main()
{
  GLFWRenderContext context = GLFWRenderContext();

  if (!context.ok)
  {
    return 1;
  }

  Application app(context);

  while (context.isRunning())
  {
    glfwPollEvents();
    app.MainLoop();
  }

  return 0;
}