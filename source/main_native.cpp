#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>

#include <nuklear.h>

#include "RenderContext/GLFWRenderContext/GLFWRenderContext.h"
#include "Renderer/Renderer.h"

int main()
{
	GLFWRenderContext context = GLFWRenderContext();
	Renderer renderer(context);

	while (context.isRunning())
	{
		glfwPollEvents();
		renderer.MainLoop();
	}

	return 0;
}