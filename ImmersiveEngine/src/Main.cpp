#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

#include"Rendering/Texture.h"
#include"Rendering/shaderClass.h"
#include"Rendering/VAO.h"
#include"Rendering/VBO.h"
#include"Rendering/EBO.h"
#include"Objects/Present.h"
#include"Math/Vector2.h"
#include"Math/Vector3.h"
#include"Objects/Present.h"

/// Decimal RGB = RGB / 255

/// Position and decimal RGB color of all vertices.
GLfloat verticesA[] =
{
	// Vertex position, Color,              Image Offset 
	// x       y  z     R     G     B       x     y 
	0.0f, -0.5f, 0.0f,	1.0f, 0.3f, 0.0f,	0.0f, 0.0f,
	//-0.0f, 0.5f, 0.0f,	0.5f, 1.0f, 0.2f,	0.0f, 1.0f,
	0.5f, 0.5f,	0.0f,	0.0f, 0.2f, 1.0f,   0.5f, 1.0f,
	1.0f, -0.5f, 0.0f,	1.0f, 0.0f, 1.0f,	1.0f, 0.0f,
};
GLfloat verticesB[] =
{
	// Vertex position, Color,              Image Offset 
	// x       y  z     R     G     B       x     y 
	-0.5f, 0.5f, 0.0f,	0.0f, 0.5f, 1.0f,	0.0f, 0.0f,
	//-0.0f, 0.5f, 0.0f,	0.5f, 1.0f, 0.2f,	0.0f, 1.0f,
	0.0f, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f,   0.5f, 1.0f,
	0.5f, 0.5f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
};

/// The order of rendering for vertices.
GLuint indices[] =
{
	0, 1, 2
};

GLfloat body[] =
{
	-0.5f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
	-0.5f, 0.5f, 0.0f,	0.0f, 0.0f,	0.0f,	0.0f, 1.0f,
	0.5f, 0.0f, 0.0f,	0.0f, 0.0f,	0.0f,	1.0f, 0.0f,
	0.5f, 0.5f, 0.0f,	0.0f, 0.0f,	0.0f,	1.0f, 1.0f,
};
GLfloat arm[] =
{
	-0.25f, -0.5f, 0.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
	-0.25f, 0.0f, 0.0f,	0.0f, 0.0f,	0.0f,	0.0f, 1.0f,
	0.25f, -0.5f, 0.0f,	0.0f, 0.0f,	0.0f,	1.0f, 0.0f,
	0.25f, 0.0f, 0.0f,	0.0f, 0.0f,	0.0f,	1.0f, 1.0f,
};

GLuint rectangle[] =
{
	0, 1, 3,
	3, 2, 0
};

Engine::Math::Vector3 move;
 
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
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Error check for GLAD.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	gladLoadGL();

	Shader shaderProgram("default.vert", "default.frag");
	
	/// Bind and unbind VAO, VBO, and EBO for initialization and packaging of VBO in VAO.
	
    //Present objA(verticesA, sizeof(verticesA), indices, sizeof(indices));
    //Present objB(verticesB, sizeof(verticesB), indices, sizeof(indices));

    //objA.setTexture("sand.jpg", GL_RGB);
    //objA.space.translate(Engine::Math::Vector3(0, 0, 1));
    //std::cout << "pos: " << objA.space.position.toString();
    //objA.space.translate(Engine::Math::Vector3(1, 0, 0));
    //std::cout << objA.toString();

	Present crabBod(body, sizeof(body), rectangle, sizeof(rectangle));
	crabBod.space.translate(Engine::Math::Vector3(0, -1, 0));
	crabBod.setTexture("crab-body.png", GL_RGBA);

	Present crabArmA(arm, sizeof(arm), rectangle, sizeof(rectangle));
	crabArmA.setTexture("crab-leg.png", GL_RGBA);
	Present crabArmB(arm, sizeof(arm), rectangle, sizeof(rectangle));
	crabArmB.setTexture("crab-leg1.png", GL_RGBA);
	
	crabArmA.space.pivotOffset = Engine::Math::Vector3(0.125f, -0.25f, 0);
	crabArmA.space.dialate(0.5f);
	crabArmA.space.translate(Engine::Math::Vector2(-0.25f, -0.6f));
	crabArmA.space.updateTransforms(shaderProgram);

	crabArmB.space.pivotOffset = Engine::Math::Vector3(-0.125f, -0.25f, 0);
	crabArmB.space.dialate(0.5f);
	crabArmB.space.translate(Engine::Math::Vector2(0.25f, -0.6f));
	crabArmB.space.updateTransforms(shaderProgram);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window); // Get inputs.

		// Rendering
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Window background in decimal RGBA
		glClear(GL_COLOR_BUFFER_BIT);
		
        shaderProgram.Activate();

        //objA.space.updateTransforms(shaderProgram);
        //objA.draw(shaderProgram);
		
		crabBod.space.updateTransforms(shaderProgram);
		crabBod.draw(shaderProgram);
        
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && crabArmA.space.orientation.z > -35)
		{
			crabArmA.space.rotate(-0.1f);
			crabArmB.space.rotate(-0.1f);
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && crabArmA.space.orientation.z < 35)
		{
			crabArmA.space.rotate(0.1f);
			crabArmB.space.rotate(0.1f);
		}

		crabArmA.space.updateTransforms(shaderProgram);
		crabArmA.draw(shaderProgram);

		crabArmB.space.updateTransforms(shaderProgram);
		crabArmB.draw(shaderProgram);

		glfwSwapBuffers(window); // Wait until next frame is rendered before switching to it.
		glfwPollEvents(); // Process window events.
	}

	shaderProgram.Delete();
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

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