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

struct TestComponent2 : public Component
{
	int y;

	TestComponent2() : y(10) {}
	TestComponent2(int y) : y(y) {}
};

int main()
{
	Logging::Init();
	
	Entity ent;
	ent.AddComponent<TestComponent>();
	ent.AddComponent<TestComponent>(10);
	
	auto component = ent.GetComponent<TestComponent>();	
	if (auto ptr = component.lock())
	{
		Logging::Debug("Component x: {}", ptr->x);
	}

	Logging::Debug("Has Component (TestComponent2): {}", ent.HasComponent<TestComponent2>());

	std::weak_ptr<TestComponent2> weakComponent;
	if (ent.TryGetComponent<TestComponent2>(weakComponent))
	{
		if (auto ptr = weakComponent.lock())
		{
			Logging::Debug("Component x: {}", ptr->y);
		}
	}
	
	auto components = ent.GetComponents<TestComponent>();
	for (const auto& cmp : components)
	{
		if (auto ptr = cmp.lock())
		{
			Logging::Debug("Component x: {}", ptr->x);
		}
	}

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
