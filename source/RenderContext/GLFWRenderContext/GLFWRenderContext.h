#include "../RenderContext.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glfw3webgpu.h>

class GLFWRenderContext final : public RenderContext
{
public:
  GLFWRenderContext();
  ~GLFWRenderContext();

  bool isRunning() override;

  // wgpu::TextureView AcquireFrame() override;
  // void Present() override;

private:
  GLFWwindow *window;
  wgpu::Surface surface;
};