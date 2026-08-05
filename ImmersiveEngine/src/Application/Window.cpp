#include"Window.h"

namespace ImmersiveEngine
{
	Window::Window(const std::string name, const Math::Vector2 dimensions) : m_name(name), m_dimensions(dimensions)
	{ }

	Window::~Window()
	{
		m_screenFBO.Delete();
		glfwDestroyWindow(m_window);
	}

	void Window::initialize()
	{
		m_window = glfwCreateWindow(m_dimensions.x, m_dimensions.y, m_name.c_str(), NULL, NULL);

		if (m_window == NULL)
		{
			std::cout << "Failed to create window (GLFW)";
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(m_window);

		//glfwSetFramebufferSizeCallback(m_window, onWindowResized);
	}

	bool Window::shouldClose()
	{
		return glfwWindowShouldClose(m_window);
	}

	void Window::close()
	{
		glfwSetWindowShouldClose(m_window, 1);
	}

	void Window::setViewport()
	{
		glViewport(0, 0, m_dimensions.x, m_dimensions.y);
		m_screenFBO.Resize(m_dimensions.x, m_dimensions.y);
	}

	void Window::swapBuffer()
	{
		glfwSwapBuffers(m_window);
	}

	ImmersiveEngine::Math::Vector2 Window::getSize()
	{
		return ImmersiveEngine::Math::Vector2();
	}

	void Window::setSize(const Math::Vector2 dimensions)
	{
		glfwSetWindowSize(m_window, dimensions.x, dimensions.y);
	}

	void Window::maximize()
	{
		glfwMaximizeWindow(m_window);
	}

	void Window::restore()
	{
		glfwRestoreWindow(m_window);
	}

	void Window::beginDraw()
	{
		m_screenFBO.Bind();
		setViewport();
	}

	void Window::render()
	{
		m_screenFBO.DrawScreen();
		swapBuffer();
	}

	void Window::endDraw()
	{
		m_screenFBO.Unbind();
	}

	void Window::onWindowResized(GLFWwindow* window, int width, int height)
	{
		m_dimensions = { (float)width, (float)height };
		setViewport();
	}
}