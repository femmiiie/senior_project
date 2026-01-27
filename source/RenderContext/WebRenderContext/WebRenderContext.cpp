#include "WebRenderContext.h"
#include <emscripten.h>
#include <emscripten/html5_webgpu.h>
#include <iostream>

WebRenderContext::WebRenderContext()
{
  this->Initialize();

  std::cout << "WebRenderContext initialized successfully" << std::endl;
}

wgpu::TextureView WebRenderContext::GetNextTextureView()
{
  wgpu::SurfaceTexture surfaceTexture;
  this->surface.getCurrentTexture(&surfaceTexture);

  if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal)
  {
    std::cerr << "Failed to get current surface texture!" << std::endl;
    return nullptr;
  }

  wgpu::TextureViewDescriptor viewDesc;
  viewDesc.setDefault();
  viewDesc.format = this->surfaceFormat;
  viewDesc.dimension = wgpu::TextureViewDimension::_2D;
  viewDesc.baseMipLevel = 0;
  viewDesc.mipLevelCount = 1;
  viewDesc.baseArrayLayer = 0;
  viewDesc.arrayLayerCount = 1;
  viewDesc.aspect = wgpu::TextureAspect::All;

  wgpu::Texture texture(surfaceTexture.texture);
  return texture.createView(viewDesc);
}

void WebRenderContext::GenerateSurface()
{
  WGPUSurfaceDescriptorFromCanvasHTMLSelector canvasDesc;
  canvasDesc.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;
  canvasDesc.selector = "#canvas";

  wgpu::SurfaceDescriptor surfDesc;
  surfDesc.nextInChain = &canvasDesc.chain;

  this->surface = this->instance.createSurface(surfDesc);
  if (!this->surface)
  {
    throw RenderContextException("Failed to create surface!");
  }
}

void WebRenderContext::GetSurfaceFormat()
{
  this->surfaceFormat = wgpu::TextureFormat::BGRA8Unorm;
}