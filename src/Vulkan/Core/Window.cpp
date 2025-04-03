#include "Window.h"
#include <GLFW/glfw3.h>

#include "Utils/Logging.hpp"

void Window::create()
{
	//glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
	glfwInit();
	glfwSetErrorCallback([](int error, const char* description) { Logging::Error("GLFW Error ({}): {}", error, description); });


	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	m_window = glfwCreateWindow(m_size.x, m_size.y, "Vulkan", nullptr, nullptr);
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
