#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"


#include "Vulkan/Core/Window.h"
#include "Renderer/Renderer.h"

#include "Utils/Logging.hpp"
#include "Utils/Time.hpp"

#include "ECS/ECS.hpp"

struct TestComponent : public Component
{
	int x;

	TestComponent() : x(5) {}
	TestComponent(int x) : x(x) {}
};

int main()
{
	Logging::Init();
	
	Entity ent;
	ent.AddComponent<TestComponent>();

	system("pause");
	
	return 0;
	Window window;
	window.create();

	Renderer renderer;
	renderer.initVulkan(&window);

	while (!glfwWindowShouldClose(window.getGLFWWindow()))
	{
		glfwPollEvents();
		renderer.drawFrame();
	}
	renderer.waitIdle();
}
