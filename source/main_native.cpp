#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>

#include "Application.h"
#include "RenderContext/GLFWRenderContext/GLFWRenderContext.h"

int main()
{
  GLFWRenderContext context = GLFWRenderContext();

  Application app(context);
  if (!app.ok)
  {
    return 1;
  }

  while (context.isRunning())
  {
    glfwPollEvents();
    app.MainLoop();
  }

  return 0;
}