#include <iostream>
#include <cassert>
#include <vector>
#include <cstring>

#include "Renderer.h"

Renderer::Renderer(RenderContext &context) : context(context)
{
  std::vector<glm::f32> data {//basic test triangle
    // position          normal               color                tex
    0.0, 0.0, 0.0, 1.0,  0.0, 0.0, 1.0, 0.0,  1.0, 0.0, 0.0, 1.0,  0.0, 0.0,
    0.5, 0.5, 0.0, 1.0,  0.0, 0.0, 1.0, 0.0,  0.0, 1.0, 0.0, 1.0,  0.0, 0.0,
    1.0, 0.0, 0.0, 1.0,  0.0, 0.0, 1.0, 0.0,  0.0, 0.0, 1.0, 1.0,  0.0, 0.0,
  };
  this->vertexCount = (glm::u32)data.size() / 14;

  this->vertexBuffer = this->CreateBuffer(
    data.size() * sizeof(glm::f32),
    wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
    false
  );

  this->context.queue.writeBuffer(this->vertexBuffer, 0, data.data(), data.size() * sizeof(glm::f32));

  this->mvp.M = glm::mat4(1);

  this->mvp.V = glm::lookAt(
    glm::vec3(0, 0, -1),
    glm::vec3(0),
    glm::vec3(0, 1, 0)
  );

  this->mvp.P = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

  this->light.position = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
  this->light.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  this->light.power = 20.0f;

  std::vector<wgpu::BindGroupLayoutEntry> bindingLayouts {
    this->CreateBindingLayout(0, wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment, utils::aligned_size(this->mvp)),
    this->CreateBindingLayout(1, wgpu::ShaderStage::Fragment, utils::aligned_size(this->light)),
  };
  this->bindGroupLayout = this->CreateBindGroupLayout(bindingLayouts);

  std::vector<wgpu::BindGroupEntry> bindings {
    this->CreateBinding(0, this->mvpBuffer, utils::aligned_size(this->mvp)), //update sizes here!!!!
    this->CreateBinding(1, this->lightBuffer, utils::aligned_size(this->light))
  };
  this->bindGroup = this->CreateBindGroup(bindings);

  this->context.queue.writeBuffer(this->mvpBuffer, 0, &this->mvp, sizeof(MVP));
  this->context.queue.writeBuffer(this->lightBuffer, 0, &this->light, sizeof(LightData));
  
  this->InitializeRenderPipeline();
}

Renderer::~Renderer()
{
  pipeline.release();
  layout.release();
  bindGroupLayout.release();
  bindGroup.release();
}

void Renderer::MainLoop()
{
  wgpu::TextureView targetView = this->context.GetNextTextureView();
  if (!targetView) { return; }

  wgpu::CommandEncoderDescriptor encoderDesc;
  this->context.InitLabel(encoderDesc.label);
  wgpu::CommandEncoder encoder = this->context.device.createCommandEncoder(encoderDesc);

  wgpu::RenderPassColorAttachment renderPassColorAttachment;
  renderPassColorAttachment.view = targetView;
  renderPassColorAttachment.resolveTarget = nullptr;
  renderPassColorAttachment.loadOp = wgpu::LoadOp::Clear;
  renderPassColorAttachment.storeOp = wgpu::StoreOp::Store;
  renderPassColorAttachment.clearValue = WGPUColor{0.0, 0.0, 0.2, 0.0};
#ifndef WEBGPU_BACKEND_WGPU
  renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif // NOT WEBGPU_BACKEND_WGPU

  wgpu::RenderPassDescriptor renderPassDesc;
  renderPassDesc.colorAttachmentCount = 1;
  renderPassDesc.colorAttachments = &renderPassColorAttachment;
  renderPassDesc.depthStencilAttachment = nullptr;
  renderPassDesc.timestampWrites = nullptr;

  wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDesc);

  renderPass.setPipeline(pipeline);
  renderPass.setVertexBuffer(0, this->vertexBuffer, 0, this->vertexBuffer.getSize());

  renderPass.setBindGroup(0, bindGroup, 0, nullptr);


  renderPass.draw(this->vertexCount, 1, 0, 0);

  
  renderPass.end();
  renderPass.release();

  wgpu::CommandBufferDescriptor cmdBufferDescriptor = {};
  this->context.InitLabel(cmdBufferDescriptor.label);

  wgpu::CommandBuffer command = encoder.finish(cmdBufferDescriptor);
  encoder.release();

  this->context.queue.submit(1, &command);
  command.release();

  targetView.release();
  this->context.Present();
  this->context.DevicePoll();
}

wgpu::Buffer Renderer::CreateBuffer(uint64_t size, wgpu::BufferUsage usage, bool mapped)
{
  wgpu::BufferDescriptor desc;
  desc.size = size;
  desc.usage = usage;
  desc.mappedAtCreation = mapped;
  return this->context.device.createBuffer(desc);
}

wgpu::BindGroupLayoutEntry Renderer::CreateBindingLayout(uint16_t binding, wgpu::ShaderStage visibility, uint64_t minBindingSize)
{
  wgpu::BindGroupLayoutEntry entry;
  entry.binding = binding;
  entry.visibility = visibility;
  entry.buffer.type = wgpu::BufferBindingType::Uniform;
  entry.buffer.minBindingSize = minBindingSize;
  return entry;
}

wgpu::BindGroupLayout Renderer::CreateBindGroupLayout(std::vector<wgpu::BindGroupLayoutEntry>& entries)
{
  wgpu::BindGroupLayoutDescriptor desc;
  desc.entryCount = entries.size();
  desc.entries = entries.data();
  return this->context.device.createBindGroupLayout(desc);
}

wgpu::BindGroupEntry Renderer::CreateBinding(uint16_t entry, wgpu::Buffer& buffer, glm::u32 size)
{
  buffer = this->CreateBuffer(
    size,
    wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
    false
  );

  wgpu::BindGroupEntry binding = wgpu::Default;
  binding.binding = entry;
  binding.buffer = buffer;
  binding.offset = 0;
  binding.size = size;
  return binding;
}

wgpu::BindGroup Renderer::CreateBindGroup(std::vector<wgpu::BindGroupEntry>& bindings)
{
  wgpu::BindGroupDescriptor desc;
  desc.layout = this->bindGroupLayout;
  desc.entryCount = bindings.size();
  desc.entries = bindings.data();
  return this->context.device.createBindGroup(desc);
}

wgpu::VertexAttribute Renderer::CreateAttribute(glm::u32 location, wgpu::VertexFormat format, uint64_t offset)
{
  wgpu::VertexAttribute attr;
  attr.shaderLocation = location;
  attr.format = format;
  attr.offset = offset;
  return attr;
}


wgpu::BlendState Renderer::GetBlendState()
{
  wgpu::BlendState state;
  state.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
  state.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
  state.color.operation = wgpu::BlendOperation::Add;
  state.alpha.srcFactor = wgpu::BlendFactor::Zero;
  state.alpha.dstFactor = wgpu::BlendFactor::One;
  state.alpha.operation = wgpu::BlendOperation::Add;
  return state;
}

void Renderer::InitializeRenderPipeline()
{
  wgpu::ShaderModule shaderModule = context.LoadShader("shader.wgsl");
  if (!shaderModule)
  {
    throw new RendererException("Failed to load shader module.");
  }

  wgpu::PipelineLayoutDescriptor layoutDesc;
  layoutDesc.bindGroupLayoutCount = 1;
  layoutDesc.bindGroupLayouts = (WGPUBindGroupLayout *)&(this->bindGroupLayout);
  this->layout = this->context.device.createPipelineLayout(layoutDesc);

  wgpu::RenderPipelineDescriptor pipelineDesc;
  pipelineDesc.layout = this->layout;

  std::vector<wgpu::VertexAttribute> vertexAttrs {
    this->CreateAttribute(0, wgpu::VertexFormat::Float32x4, 0),
    this->CreateAttribute(1, wgpu::VertexFormat::Float32x4, 4 * sizeof(glm::f32)),
    this->CreateAttribute(2, wgpu::VertexFormat::Float32x4, 8 * sizeof(glm::f32)),
    this->CreateAttribute(3, wgpu::VertexFormat::Float32x2, 12 * sizeof(glm::f32)),
  };

  wgpu::VertexBufferLayout vertexBufferLayout;
  vertexBufferLayout.attributeCount = vertexAttrs.size();
  vertexBufferLayout.attributes = vertexAttrs.data();
  vertexBufferLayout.arrayStride = 14 * sizeof(glm::f32);
  vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

  pipelineDesc.vertex.bufferCount = 1;
  pipelineDesc.vertex.buffers = &vertexBufferLayout;
  pipelineDesc.vertex.module = shaderModule;
  pipelineDesc.vertex.constantCount = 0;
  pipelineDesc.vertex.constants = nullptr;
  this->context.SetEntryPoint(pipelineDesc.vertex.entryPoint, "vs_main");


  pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
  pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
  pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
  pipelineDesc.primitive.cullMode = wgpu::CullMode::Back;

  wgpu::BlendState blend = this->GetBlendState();

  wgpu::ColorTargetState colorTarget;
  colorTarget.format = this->context.surfaceFormat;
  colorTarget.blend = &blend;
  colorTarget.writeMask = wgpu::ColorWriteMask::All;

  wgpu::FragmentState fragmentState;
  fragmentState.module = shaderModule;
  fragmentState.constantCount = 0;
  fragmentState.constants = nullptr;
  fragmentState.targetCount = 1;
  fragmentState.targets = &colorTarget;
  this->context.SetEntryPoint(fragmentState.entryPoint, "fs_main");

  pipelineDesc.fragment = &fragmentState;

  pipelineDesc.depthStencil = nullptr;
  pipelineDesc.multisample.count = 1;
  pipelineDesc.multisample.mask = ~0u;
  pipelineDesc.multisample.alphaToCoverageEnabled = false;

  pipeline = this->context.device.createRenderPipeline(pipelineDesc);
  shaderModule.release();
}