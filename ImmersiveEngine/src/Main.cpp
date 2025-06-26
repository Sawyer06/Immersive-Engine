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

#include"Settings.h"
#include"Rendering/Texture.h"
#include"Rendering/shaderClass.h"
#include"Objects/Present.h"
#include"Components/Camera.h"
#include"Components/Light.h"
#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include"Objects/Present.h"
#include"Rendering/Mesh.h"
#include"Components/Space.h"

/// Decimal RGB = RGB / 255

/// Position and decimal RGB color of all vertices.
GLfloat verticesA[] =
{
	// Vertex position,		Color,					 Image Offset	  Normal direction
	// x       y  z			R     G     B			 x     y		  x     y      z
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,      0.0f, -1.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,      0.0f, -1.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, -1.0f, 0.0f,

	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,     -0.8f, 0.5f,  0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,     -0.8f, 0.5f,  0.0f, 
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,     -0.8f, 0.5f,  0.0f, 

	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f, -0.8f, 
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.0f, 0.5f, -0.8f, 
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f, -0.8f, 

	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.8f, 0.5f,  0.0f, 
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.8f, 0.5f,  0.0f, 
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.8f, 0.5f,  0.0f, 

	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f,  0.8f, 
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, 0.5f,  0.8f, 
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f,  0.8f  
};

/// The order of rendering for vertices.
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	4, 6, 5,
	7, 9, 8,
	10, 12, 11,
	13, 15, 14
};
 
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(ImmersiveEngine::Settings::g_screenLength, ImmersiveEngine::Settings::g_screenWidth, "HelloWorld", NULL, NULL); // Create window.
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

	//ImmersiveEngine::Settings::g_ambientLightColor = ImmersiveEngine::Math::Vector3(255, 0, 0);

	ImmersiveEngine::cbs::Present cam;
	ImmersiveEngine::cbs::Camera* camComp = cam.addComponent<ImmersiveEngine::cbs::Camera>();
	cam.space->position = ImmersiveEngine::Math::Vector3(2.0f, -0.3f, 2);
	cam.space->rotate(ImmersiveEngine::Math::Vector3(-1.0f, 0.5f, 0));

	auto pyramidMesh = std::make_shared<Mesh>(verticesA, sizeof(verticesA), indices, sizeof(indices));
	//auto pyramid = std::make_shared<ImmersiveEngine::cbs::Present>("Pyramid", pyramidMesh);
	ImmersiveEngine::cbs::Present pyramid("Pyramid", pyramidMesh);
	Texture* sand = new Texture("sand.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	pyramid.mesh->setTexture(sand);
	
	ImmersiveEngine::cbs::Present lightA;
	ImmersiveEngine::cbs::Space* spaceComp = lightA.getComponent<ImmersiveEngine::cbs::Space>();
	ImmersiveEngine::cbs::Light* lightComp = lightA.addComponent<ImmersiveEngine::cbs::Light>(ImmersiveEngine::Math::Vector3(255, 0, 0), 1.0f);
	lightA.space->position.y += -0.2f;
	lightA.space->position.x += 1;
	lightComp->specular.color = ImmersiveEngine::Math::Vector3(0,0, 255);
	//lightComp->diffuse.color = ImmersiveEngine::Math::Vector3(200, 255, 0);
	//lightComp->specular.intensity = 1.0f;
	lightComp->diffuse.intensity = 1.2f;

	float speed = 0.1f;
	ImmersiveEngine::Math::Vector3 dir;
	ImmersiveEngine::Math::Vector3 color(255, 0, 0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window); // Get inputs.
		camComp->refreshViewProjection(shaderProgram, (float)ImmersiveEngine::Settings::g_screenLength / ImmersiveEngine::Settings::g_screenWidth);

		// Rendering
		glClearColor(0.0f, 0.4f, 0.8f, 0.0f); // Window background in decimal RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
        shaderProgram.Activate();
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			pyramid.space->rotate(ImmersiveEngine::Math::Vector3(0, 0.01f, 0));
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			lightA.space->translate(ImmersiveEngine::Math::Vector3(0, 0.001f, 0));
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			lightA.space->translate(ImmersiveEngine::Math::Vector3(0, -0.001f, 0));
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			lightA.space->translate(ImmersiveEngine::Math::Vector3(-0.001f, 0, 0));
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			lightA.space->translate(ImmersiveEngine::Math::Vector3(0.001f, 0, 0));
		}
		//if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) cam->space.rotate(ImmersiveEngine::Math::Vector3(0.001f, 0, 0)); gimble locks
		//if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) cam->space.rotate(ImmersiveEngine::Math::Vector3(-0.001f, 0, 0));
		
		dir.y = color.x > 0 && color.z < 255 ? speed : -speed;
		dir.x = color.z > 0 && color.y < 255 ? speed : -speed;
		dir.z = color.y > 0 && color.x < 255  ? speed : -speed;

		if (color.x > 255) color.x = 255;
		if (color.x < 0) color.x = 0;
		if (color.y > 255) color.y = 255;
		if (color.y < 0) color.y = 0;
		if (color.z > 255) color.z = 255;
		if (color.z < 0) color.z = 0;

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			color += dir;
		}
		lightComp->diffuse.color = color;
		lightComp->mainColor = color;

		lightComp->refreshLight(shaderProgram, spaceComp->position);

		pyramid.space->refreshTransforms(shaderProgram);
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