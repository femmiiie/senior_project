#ifndef RENDERER_H
#define RENDERER_H

#include <webgpu/webgpu.hpp>

#include <iostream>
#include <cassert>
#include <vector>
#include <cstring>
#include <array>

#include "../RenderContext/RenderContext.h"
#include "../Utils/Utils.h"

typedef std::array<float_t, 2> vec2f;
typedef std::array<float_t, 3> vec3f;
typedef std::array<float_t, 4> vec4f;
typedef std::array<float_t, 16> mat4f;


class Renderer
{
public:
  Renderer(RenderContext &context);
  ~Renderer();
  void MainLoop();

private:
  struct LightData
  {
    vec4f position;
    vec4f color;
    float_t power;
  };

  struct MVP
  {
    mat4f M;
    mat4f V;
    mat4f P;
  };

  LightData light;
  MVP mvp;

  void InitializeRenderPipeline();

  wgpu::Buffer CreateBuffer(uint64_t size, wgpu::BufferUsage usage, bool mapped);

  wgpu::BindGroupLayoutEntry CreateBindingLayout(uint16_t binding, wgpu::ShaderStage visibility, uint64_t minBindingSize);
  wgpu::BindGroupLayout CreateBindGroupLayout(std::vector<wgpu::BindGroupLayoutEntry>& entries);

  wgpu::BindGroupEntry CreateBinding(uint16_t entry, wgpu::Buffer& buffer, uint32_t size);
  wgpu::BindGroup CreateBindGroup(std::vector<wgpu::BindGroupEntry>& bindings);
  
  wgpu::VertexAttribute CreateAttribute(uint32_t location, wgpu::VertexFormat format, uint64_t offset = 0);
  wgpu::BlendState GetBlendState();


  RenderContext &context;
  wgpu::RenderPipeline pipeline;
  wgpu::Buffer lightBuffer;
  wgpu::Buffer mvpBuffer;

  uint32_t vertexCount;
  wgpu::Buffer vertexBuffer;

  wgpu::PipelineLayout layout;
  wgpu::BindGroupLayout bindGroupLayout;
  wgpu::BindGroup bindGroup;
};

class RendererException : public std::exception
{
private:
  std::string message;

public:
  RendererException(std::string m) : message(m) {}
  RendererException(const char *m) { this->message = m; }
  const char *what() const noexcept override { return this->message.c_str(); }
};

#endif