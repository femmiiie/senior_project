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

  bool isRunning();

  void GenerateSurface() override;
  void GetSurfaceFormat() override;
  void SetEntryPoint(WGPUStringView &entryPoint, const char* name) override;
  void InitLabel(WGPUStringView &label) override { label = WGPU_STRING_VIEW_INIT; };
  wgpu::ShaderModule CreateShaderModuleFromSource(std::string &shaderSource) override;

  wgpu::TextureView GetNextTextureView() override;
  void Present() override;
  void DevicePoll() override;

private:
  GLFWwindow *window;
};
