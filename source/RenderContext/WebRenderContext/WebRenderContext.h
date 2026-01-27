#ifndef WEBRENDERCONTEXT
#define WEBRENDERCONTEXT

#include "../RenderContext.h"

class WebRenderContext : public RenderContext
{
public:
  WebRenderContext();

  void GenerateSurface() override;
  void GetSurfaceFormat() override;

  wgpu::TextureView GetNextTextureView() override;
  void Present() override {};
  void DevicePoll() override {};
};

#endif