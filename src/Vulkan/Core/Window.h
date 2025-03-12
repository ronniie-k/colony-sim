#pragma once

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

private:
	static void onResize(GLFWwindow* window, int width, int height);

private:
	GLFWwindow* m_window;
	bool m_resized = false;
};