#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_uint2.hpp>
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window
{
public:
	void create();
	void destroy();
	GLFWwindow* getGLFWWindow() { return m_window; }

	bool hasResized() const { return m_resized; }
	void setResized(bool resized) { m_resized = resized; }

	const glm::vec2 getSize() const { return m_size; }

private:
	static void onResize(GLFWwindow* window, int width, int height);

private:
	GLFWwindow* m_window;
	bool m_resized = false;
	const glm::vec2 m_size = { 800, 600 };
};