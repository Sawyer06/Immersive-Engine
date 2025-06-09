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
#include"Components/Mesh.h"

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

GLfloat coconut[] =
{
	-0.25f, 0.5f, 0.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
	-0.25f, 1.0f, 0.0f,	0.0f, 0.0f,	0.0f,	0.0f, 1.0f,
	0.25f, 0.5f, 0.0f,	0.0f, 0.0f,	0.0f,	1.0f, 0.0f,
	0.25f, 1.0f, 0.0f,	0.0f, 0.0f,	0.0f,	1.0f, 1.0f,
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

float waitTime = 0;
std::list<Present> coconuts;
int score = 0;
 
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void dropWait();
void spawnCoconut(Present coconutObj);

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

	Texture bodTexture("crab-body.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	Texture armTextureA("crab-leg.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	Texture armTextureB("crab-leg1.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	Texture coconutTexture("coconut.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);

	Mesh* crabMesh = new Mesh(body, sizeof(body), rectangle, sizeof(rectangle));
	Present crabBod("crab_0", crabMesh);
	crabBod.space.translate(Engine::Math::Vector3(0, -1, 0));
	crabBod.mesh->setTexture(&bodTexture);
	
	Mesh* crabArmMeshA = new Mesh(arm, sizeof(arm), rectangle, sizeof(rectangle));
	Present crabArmA("crab_1", crabArmMeshA);
	crabArmA.mesh->setTexture(&armTextureA);
	Mesh* crabArmMeshB = new Mesh(arm, sizeof(arm), rectangle, sizeof(rectangle));
	Present crabArmB("crab_2", crabArmMeshB);
	crabArmB.mesh->setTexture(&armTextureB);
	
	crabArmA.space.pivotOffset = Engine::Math::Vector3(0.125f, -0.25f, 0);
	crabArmA.space.dialate(0.5f);
	crabArmA.space.translate(Engine::Math::Vector2(-0.25f, -0.6f));
	crabArmA.space.updateTransforms(shaderProgram);

	crabArmB.space.pivotOffset = Engine::Math::Vector3(-0.125f, -0.25f, 0);
	crabArmB.space.dialate(0.5f);
	crabArmB.space.translate(Engine::Math::Vector2(0.25f, -0.6f));
	crabArmB.space.updateTransforms(shaderProgram);

	Mesh* coconutMesh = new Mesh(coconut, sizeof(coconut), rectangle, sizeof(rectangle));
	Present coconutObj("coconut", coconutMesh);
	coconutObj.mesh->setTexture(&coconutTexture);

	srand(time(0));
	dropWait();

	while (!glfwWindowShouldClose(window))
	{
		processInput(window); // Get inputs.

		// Rendering
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Window background in decimal RGBA
		glClear(GL_COLOR_BUFFER_BIT);
		
        shaderProgram.Activate();

        //objA.space.updateTransforms(shaderProgram);
        //objA.draw(shaderProgram);
		if (waitTime > 0)
		{
			waitTime -= 0.003f;
		}
		else
		{
			spawnCoconut(coconutObj);
		}

		coconuts.erase( // Destroy objects that are not enabled anymore.
			std::remove_if(coconuts.begin(), coconuts.end(),
				[](const Present& c) {
					return !c.enabled;
				}),
			coconuts.end()
		);

		for (Present& c : coconuts)
		{
			if (!c.enabled) continue; // Do not draw objects not enabled.
			c.space.translate(Engine::Math::Vector2(0, -0.0005f)); // Constantly fall.
			c.space.updateTransforms(shaderProgram);
			c.mesh->draw(shaderProgram);
			if (crabArmA.isCollidingWith(c) || crabArmB.isCollidingWith(c)) // If the object collides with an arm.
			{
				score++;
				c.enabled = false;
				std::cout << "Score: " << score << std::endl;
			}
			else if (c.space.position.y < -2) // Disable objects out of range.
			{
				c.enabled = false;
			}
		}

		crabBod.space.updateTransforms(shaderProgram);
		crabBod.mesh->draw(shaderProgram);
        
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && crabArmA.space.orientation.z > -35) // Right
		{
			crabArmA.space.rotate(-0.1f);
			crabArmB.space.rotate(-0.1f);
			crabArmA.space.position.x = -0.25f;
			crabArmB.space.position.x = 0.27f;
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && crabArmA.space.orientation.z < 35) // Left
		{
			crabArmA.space.rotate(0.1f);
			crabArmB.space.rotate(0.1f);
			crabArmA.space.position.x = -0.27f;
			crabArmB.space.position.x = 0.25f;
		}

		crabArmA.space.updateTransforms(shaderProgram);
		crabArmA.mesh->draw(shaderProgram);

		crabArmB.space.updateTransforms(shaderProgram);
		crabArmB.mesh->draw(shaderProgram);

		glfwSwapBuffers(window); // Wait until next frame is rendered before switching to it.
		glfwPollEvents(); // Process window events.
	}
	coconuts.clear();

	delete crabMesh;
	delete crabArmMeshA;
	delete crabArmMeshB;
	delete coconutMesh;

	shaderProgram.Delete();
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void dropWait()
{
	//std::cout << "Waiting!";
	
	waitTime = 3 + rand() % 10;
}

void spawnCoconut(Present coconutObj)
{
	//std::cout << "Spawning!";
	Present coconut = coconutObj;
	coconuts.emplace_back(coconut);

	coconut.space.dialate(0.15f);
	coconut.space.translate(Engine::Math::Vector3(0, 0.5f, 0));
	int side = rand() % 2;
	if (side == 0)
	{
		coconut.space.translate(Engine::Math::Vector2(-0.33f, 0));
	}
	else
	{
		coconut.space.translate(Engine::Math::Vector2(0.5f, 0));
	}
	dropWait();
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