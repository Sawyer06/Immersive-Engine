#include<iostream>
#include<memory>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <list>

#include"Rendering/Texture.h"
#include"Rendering/shaderClass.h"
#include"Objects/Present.h"
#include"Objects/Camera.h"
#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include"Objects/Present.h"
#include"Rendering/Mesh.h"

/// Decimal RGB = RGB / 255

/// Position and decimal RGB color of all vertices.
GLfloat verticesA[] =
{
	// Vertex position, Color,              Image Offset 
	// x       y  z     R     G     B       x     y 
	-0.5f, 0.0f, 0.5f,	1.0f, 0.3f, 1.0f,	0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,	0.5f, 1.0f, 0.2f,	5.0f, 0.0f,
	0.5f, 0.0f,	-0.5f,	0.3f, 0.2f, 1.0f,   0.0f, 0.0f,
	0.5f, 0.0f, 0.5f,	0.0f, 0.7f, 1.0f,	5.0f, 0.0f,
	0.0f, 0.8f, 0.0f,	0.8f, 0.0f, 1.0f,	2.5f, 5.0f
};

/// The order of rendering for vertices.
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};
 
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "HelloWorld", NULL, NULL); // Create window.
	// Error check if window isn't created.
	if (window == NULL)
	{
		std::cout << "Failed to create window (GLFW)";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback); // Calls method when window is resized.

	// Error check for GLAD.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	gladLoadGL();
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shaderProgram("default.vert", "default.frag");

	ImmersiveEngine::cbs::Camera cam;
	
	auto pyramidMesh = std::make_shared<Mesh>(verticesA, sizeof(verticesA), indices, sizeof(indices));
	ImmersiveEngine::cbs::Present pyramid("Pyramid", pyramidMesh);
	Texture* sand = new Texture("sand.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	pyramid.mesh->setTexture(sand);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window); // Get inputs.
		cam.refreshViewProjection(shaderProgram, (float)800 / 800);

		// Rendering
		glClearColor(0.5f, 1.0f, 1.0f, 0.0f); // Window background in decimal RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
        shaderProgram.Activate();

		pyramid.space.rotate(ImmersiveEngine::Math::Vector3(0, 0.01f, 0));
		
		pyramid.space.refreshTransforms(shaderProgram);
		pyramid.mesh->draw(shaderProgram);

		glfwSwapBuffers(window); // Wait until next frame is rendered before switching to it.
		glfwPollEvents(); // Process window events.
	}
	shaderProgram.Delete();
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

/// Stretch frame contents on window resize.
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // On escape key.
	{
		glfwSetWindowShouldClose(window, true);
	}
}