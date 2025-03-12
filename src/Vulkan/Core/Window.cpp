#include "Window.h"
#include <GLFW/glfw3.h>


void Window::create()
{
	const uint32_t width = 800;
	const uint32_t height = 600;

	glfwInit();
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	m_window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, Window::onResize);
}

void Window::destroy()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Window::onResize(GLFWwindow* window, int width, int height)
{
	auto self = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	self->m_resized = true;
}
