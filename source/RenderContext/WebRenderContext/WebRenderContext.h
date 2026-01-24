#ifndef WEBRENDERCONTEXT
#define WEBRENDERCONTEXT

#include "../RenderContext.h"

class WebRenderContext : public RenderContext
{
public:
  WebRenderContext();

  wgpu::TextureView GetNextTextureView() override;
  void Present() override {};
  void DevicePoll() override {};
};

#endif