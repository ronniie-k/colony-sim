#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "Vulkan/Core/Window.h"
#include "Renderer/Renderer.h"

#include "Utils/Logging.hpp"
#include "Utils/Time.hpp"

#include "Renderer/Shape/Quad.h"

#include <unordered_map>

int main()
{
	Time::Init();
	Logging::Init();
	Window window;
	window.create();

	Renderer renderer;
	renderer.initVulkan(&window);

	enum class colorCodes : int
	{
		eWhite = 0,
		eBlack,
		eBlue,
		eOrange
	};

	std::unordered_map<int, glm::vec3> colors = {
		{ 0, { 1, 1, 1 } },
		{ 1, { 0, 0, 0 } },
		{ 2, { 0.1, 0.23, 0.8 } },
		{ 3, { 0, 0, 0 } },
	};

	std::vector<Quad> quads;

	for (int i = 0; i < 10; i++)
	{
		Quad q;
		q.setPosition({ i * 50, 0 });
		q.setScale({ 3, 3 });
		q.setColor(colors[(i % 2) + 2]);
		q.setRotation(0);
		quads.emplace_back(std::move(q));
	}

	Quad quad;
	quad.setPosition({ 220, 0 });
	quad.setScale({ 3, 3 });
	quad.setColor({ 0.8, 0.23, 0.1 });
	quad.setRotation(0);

	Quad quad2;
	quad2.setPosition({ -220, 220 });
	quad2.setScale({ 3, 3 });
	quad2.setColor({ 0.8, 0.23, 0.1 });
	quad2.setRotation(0);

	while (!glfwWindowShouldClose(window.getGLFWWindow()))
	{
		Time::Update();
		glfwPollEvents();
		renderer.draw(quad);
		renderer.draw(quad2);
		for (auto& quad : quads)
		{
			; // renderer.draw(quad);
		}
		renderer.drawFrame();
		// quad.move({ 0.00001f, 0.f });
	}
	renderer.waitIdle();
}

/*
mainloop:

while(running)
{
	renderer.addToQueue(quad);
	renderer.addToQueue(quad);
	renderer.addToQueue(quad);

	renderer.drawFrame();
}

*/

// https://www.reddit.com/r/gameenginedevs/comments/1in9exd/you_only_realise_what_youve_lost_when_its_gone/
// https://www.reddit.com/r/vulkan/comments/167jh7l/renderdoc_vulkan_extensions_on_linux/