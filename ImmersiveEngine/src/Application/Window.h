#ifndef WINDOW_CLASS_H
#define WINDOW_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"../Math/Vector2.h"
#include"../Rendering/FBO.h"

namespace ImmersiveEngine
{
	class Window
	{
		public:
			Window(const std::string name = "ImmersiveEngine", const Math::Vector2 dimensions = {1920, 1080});
			~Window();
			
			void initialize();

			bool shouldClose();
			void close();

			void setViewport();
			void swapBuffer();

			ImmersiveEngine::Math::Vector2 getSize();
			void setSize(const Math::Vector2 dimensions);

			void maximize();
			void restore();

			void beginDraw();
			void render();
			void endDraw();
		
		private:
			Math::Vector2 m_dimensions;
			std::string m_name;

			GLFWwindow* m_window;
			Rendering::FBO m_screenFBO;

			void onWindowResized(GLFWwindow* window, int width, int height);
	};
}
#endif