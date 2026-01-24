#include "WebRenderContext.h"

WebRenderContext::WebRenderContext()
{
}

wgpu::TextureView WebRenderContext::GetNextTextureView()
{
  // Get the surface texture
  wgpu::SurfaceTexture surfaceTexture;
  this->surface.getCurrentTexture(&surfaceTexture);
  if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal)
  {
    return nullptr;
  }
  wgpu::Texture texture = surfaceTexture.texture;

  // Create a view for this surface texture
  wgpu::TextureViewDescriptor viewDescriptor;
  viewDescriptor.label = WGPU_STRING_VIEW_INIT;
  viewDescriptor.format = texture.getFormat();
  viewDescriptor.dimension = wgpu::TextureViewDimension::_2D;
  viewDescriptor.baseMipLevel = 0;
  viewDescriptor.mipLevelCount = 1;
  viewDescriptor.baseArrayLayer = 0;
  viewDescriptor.arrayLayerCount = 1;
  viewDescriptor.aspect = wgpu::TextureAspect::All;
  wgpu::TextureView targetView = texture.createView(viewDescriptor);

#ifndef WEBGPU_BACKEND_WGPU
  // We no longer need the texture, only its view
  // (NB: with wgpu-native, surface textures must not be manually released)
  wgpu::Texture(surfaceTexture.texture).release();
#endif // WEBGPU_BACKEND_WGPU

  return targetView;
}