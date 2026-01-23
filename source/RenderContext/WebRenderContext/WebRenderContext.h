#ifndef WEBRENDERCONTEXT
#define WEBRENDERCONTEXT

#include "../RenderContext.h"

class WebRenderContext : public RenderContext
{
public:
  WebRenderContext();

  // wgpu::TextureView AcquireFrame() override;
  // void Present() override;
};

#endif