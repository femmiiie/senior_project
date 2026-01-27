#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>

#include <emscripten.h>

#include "Application.h"
#include "RenderContext/WebRenderContext/WebRenderContext.h"

int main()
{
  WebRenderContext context = WebRenderContext();

  Application app(context);
  if (!app.ok)
  {
    return 1;
  }

  auto callback = [](void *arg)
  {
    Application *app = reinterpret_cast<Application *>(arg);
    app->MainLoop();
  };
  emscripten_set_main_loop_arg(callback, &app, 0, true);

  return 0;
}