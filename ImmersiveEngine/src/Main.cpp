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
#include"Math/Quaternion.h"
#include"Objects/Present.h"
#include"Rendering/Mesh.h"
#include"Components/Space.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::string windowTitle = "HelloWorld";
	GLFWwindow* window = glfwCreateWindow(ImmersiveEngine::Settings::g_screenLength, ImmersiveEngine::Settings::g_screenHeight, windowTitle.c_str(), NULL, NULL); // Create window.
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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shaderProgram("default.vert", "default.frag");

	//ImmersiveEngine::Settings::g_ambientLightColor = ImmersiveEngine::Math::Vector3(255, 0, 0);

	ImmersiveEngine::cbs::Present cam;
	ImmersiveEngine::cbs::Camera* camComp = cam.addComponent<ImmersiveEngine::cbs::Camera>();
	cam.space->position = ImmersiveEngine::Math::Vector3(0, 0, 2);
	//cam.space->position = ImmersiveEngine::Math::Vector3(2.0f, 1.0f, 2);
	//cam.space->rotate(ImmersiveEngine::Math::Vector3(-1.0f, -0.5f, 0));

	Texture* sand = new Texture("sand.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	Texture* stone = new Texture("stone.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);

	auto planeMesh = std::make_shared<Mesh>(Mesh::generatePlane(3, 5));
	
	ImmersiveEngine::cbs::Present plane("Plane", planeMesh);
	plane.mesh->setTexture(stone);
	plane.space->dialate(5.0f);
	plane.space->translate(ImmersiveEngine::Math::Vector3(0.0f, -2.0f, -6.0f));

	auto primitiveMesh = std::make_shared<Mesh>(Mesh::generateCube(1));
	auto primitiveMeshB = std::make_shared<Mesh>(Mesh::generateSquarePyramid(0.5f, 0.5f));

	ImmersiveEngine::cbs::Present lightA;
	ImmersiveEngine::cbs::Space* spaceComp = lightA.getComponent<ImmersiveEngine::cbs::Space>();
	ImmersiveEngine::cbs::Light* lightComp = lightA.addComponent<ImmersiveEngine::cbs::Light>(ImmersiveEngine::Math::Vector3(255, 255, 255), 1.0f);
	lightA.space->position.y += -0.2f;
	lightA.space->position.x += 1;
	lightA.space->position.z += 1;

	ImmersiveEngine::cbs::Present primitive("Prim", primitiveMesh);
	primitive.mesh->setTexture(sand);
	primitive.space->dialate(1.0f);
	primitive.space->translate(ImmersiveEngine::Math::Vector3(0.0f, 1.0f, -2.0f));

	//primitive.space->rotate(90, ImmersiveEngine::Math::Vector3::up);
	//primitive.space->pivotPoint.x -= 0.5f;
	//primitive.space->pivotPoint.y -= 0.5f;
	//lightComp->specular.color = ImmersiveEngine::Math::Vector3(0, 0, 0);
	//lightComp->diffuse.color = ImmersiveEngine::Math::Vector3(200, 255, 0);
	//lightComp->specular.intensity = 1.0f;
	//lightComp->diffuse.intensity = 0.6f;
	float camSpeed = 0.002f;

	double mouseX = ImmersiveEngine::Settings::g_screenLength / 2;
	double mouseY = ImmersiveEngine::Settings::g_screenHeight / 2;
	double lastX = mouseX;
	double lastY = mouseY;
	float sensitivity = 0.2f;
	float pitch = 0;

	bool clickIn = false;

	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	uint32_t counter = 0;

	while (!glfwWindowShouldClose(window))
	{
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;
		if (timeDiff >= 1.0 / 30.0)
		{
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string newTitle = windowTitle + " : " + FPS + " FPS";
			glfwSetWindowTitle(window, newTitle.c_str());
			prevTime = crntTime;
			counter = 0;
		}

		processInput(window); // Get inputs.
		camComp->refreshViewProjection(shaderProgram, (float)ImmersiveEngine::Settings::g_screenLength / ImmersiveEngine::Settings::g_screenHeight);
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Window background in decimal RGBA
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
        shaderProgram.Activate();

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			clickIn = true;
			glfwSetCursorPos(window, (double)(ImmersiveEngine::Settings::g_screenHeight / 2), (double)(ImmersiveEngine::Settings::g_screenLength / 2));
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			clickIn = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		if (clickIn)
		{
			glfwGetCursorPos(window, &mouseX, &mouseY);
			float rotX = lastX - mouseX;
			float rotY = lastY - mouseY;

			pitch += rotY * sensitivity; // Clamping

			if (rotX != 0)
			{
				cam.space->rotate(rotX * sensitivity, ImmersiveEngine::Math::Vector3::up);
			}
			if (rotY != 0 && pitch > -80.0f && pitch < 70.0f)
			{
				cam.space->rotate(rotY * sensitivity, -cam.space->getRight());
			}

			lastX = mouseX;
			lastY = mouseY;
		}


		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			cam.space->rotate(-0.05f, ImmersiveEngine::Math::Vector3::up);
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			cam.space->rotate(0.05f, ImmersiveEngine::Math::Vector3::up);
		}
		else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && cam.space->orientation.y < 0.4f)
		{
			cam.space->rotate(-0.05f, cam.space->getRight());
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && cam.space->orientation.y > -0.4f)
		{
			cam.space->rotate(0.05f, cam.space->getRight());
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			cam.space->translate(-cam.space->getForward() * camSpeed);
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			cam.space->translate(cam.space->getForward() * camSpeed);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			cam.space->translate(cam.space->getRight() * camSpeed);
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			cam.space->translate(-cam.space->getRight() * camSpeed);
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			cam.space->translate(ImmersiveEngine::Math::Vector3::up * camSpeed);
		}
		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		{
			if (cam.space->position.y > 0.0f)
			{
				cam.space->translate(-ImmersiveEngine::Math::Vector3::up * camSpeed);
			}
			else
			{
				cam.space->position.y = 0;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			camSpeed = 0.008f;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			camSpeed = 0.003f;
		}

		//primitive.space->lookAt(cam.space->position);
		//primitive.space->rotate(0.05f, ImmersiveEngine::Math::Vector3::right);
		//primitive.space->rotate(0.1f, ImmersiveEngine::Math::Vector3::forward);

		cam.space->refreshTransforms(shaderProgram);

		lightComp->refreshLight(shaderProgram, spaceComp->position);
		lightA.space->refreshTransforms(shaderProgram);
		//lightA.mesh->draw(shaderProgram);

		plane.space->refreshTransforms(shaderProgram);
		plane.mesh->draw(shaderProgram);

		primitive.space->refreshTransforms(shaderProgram);
		primitive.mesh->draw(shaderProgram);

		glfwSwapBuffers(window); // Wait until next frame is rendered before switching to it.
		glfwPollEvents(); // Process window events.
	}
	sand->Delete();
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