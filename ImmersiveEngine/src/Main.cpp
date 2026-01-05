#include<iostream>
#include<memory>

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
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
#include"Rendering/FBO.h"
#include"Managers/LightingManager.h"

#include"XR/OpenXRManager.h"

#ifdef _WIN32
extern "C"
{
	// Force NVIDIA GPU usage
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	// Force AMD GPU usage  
	//__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::string windowTitle = "HelloWorld";
	GLFWwindow* window = glfwCreateWindow(ImmersiveEngine::Settings::g_screenWidth, ImmersiveEngine::Settings::g_screenHeight, windowTitle.c_str(), NULL, NULL); // Create window.
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
	glfwSwapInterval(0); // vsync off

	bool openInVR = false;
	std::vector<FBO> eyeFBO;
	ImmersiveEngine::XR::OpenXRManager xr;
	if (openInVR)
	{
		xr.establishConnection();
		eyeFBO.resize(xr.getEyeCount());
		
		xr.input.createActionBindings();
	}

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shaderProgram("default.vert", "default.frag");
	Shader screenShader("screen.vert", "screen.frag");
	
	//screenShader.Activate();

	FBO FBO;
	FBO.Resize(ImmersiveEngine::Settings::g_screenWidth, ImmersiveEngine::Settings::g_screenHeight);

	ImmersiveEngine::cbs::Present cam;
	ImmersiveEngine::cbs::Camera* camComp = cam.addComponent<ImmersiveEngine::cbs::Camera>();
	cam.space->position = ImmersiveEngine::Math::Vector3(0, 0, 2);
	//cam.space->position = ImmersiveEngine::Math::Vector3(2.0f, 1.0f, 2);
	//cam.space->rotate(ImmersiveEngine::Math::Vector3(-1.0f, -0.5f, 0));

	std::shared_ptr<Texture> deadGrassTex = std::make_shared<Texture>("dead-grass.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<Texture> cobblestoneTex = std::make_shared<Texture>("cobblestone.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<Texture> facadeATex = std::make_shared<Texture>("abandoned-building1.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<Texture> facadeBTex = std::make_shared<Texture>("abandoned-building2.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<Texture> facadeCTex = std::make_shared<Texture>("abandoned-building3.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<Texture> facadeDTex = std::make_shared<Texture>("abandoned-building4.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<Texture> facadeETex = std::make_shared<Texture>("abandoned-building5.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<Texture> facadeFTex = std::make_shared<Texture>("abandoned-building6.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<Texture> facadeGTex = std::make_shared<Texture>("abandoned-building7.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<Texture> facadeHTex = std::make_shared<Texture>("abandoned-building8.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<Texture> facadeITex = std::make_shared<Texture>("abandoned-building9.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);

	auto planeMesh1 = std::make_shared<Mesh>(Mesh::generatePlane(15, 4));
	ImmersiveEngine::cbs::Present planeA("Plane_1", planeMesh1);
	planeA.mesh->setTexture(cobblestoneTex);
	planeA.mesh->textureScale = 1.2f;
	planeA.space->dialate(5.0f);
	planeA.space->translate(ImmersiveEngine::Math::Vector3(0.0f, -2.0f, -10.0f));
	ImmersiveEngine::cbs::Present planeB("Plane_2", planeMesh1);
	planeB.space->dialate(5.0f);
	planeB.space->translate(ImmersiveEngine::Math::Vector3(47.5f, -2.0f, 10.0f));
	planeB.space->rotate(90.0f, ImmersiveEngine::Math::Vector3::up);

	auto planeMesh2 = std::make_shared<Mesh>(Mesh::generatePlane(15, 10));
	ImmersiveEngine::cbs::Present planeC("Plane_3", planeMesh2);
	planeC.mesh->setTexture(deadGrassTex);
	planeC.mesh->textureScale = 0.6f;
	planeC.space->dialate(5.0f);
	planeC.space->translate(ImmersiveEngine::Math::Vector3(0.0f, -2.0f, 25.0f));

	auto wallMesh1 = std::make_shared<Mesh>(Mesh::generateSquare(1));
	ImmersiveEngine::cbs::Present wallA("Facade_1", wallMesh1);
	wallA.mesh->setTexture(facadeATex);
	wallA.mesh->textureScale = 1.0f;
	wallA.mesh->textureOffset.y -= 0.5f;
	wallA.space->dialate(12.0f);
	wallA.space->translate(ImmersiveEngine::Math::Vector3(0.0f, 4.0f, -16.0f));

	auto wallMesh2 = std::make_shared<Mesh>(Mesh::generateCube(1));
	ImmersiveEngine::cbs::Present wallB("Facade_2", wallMesh2);
	wallB.mesh->setTexture(facadeDTex);
	wallB.mesh->textureOffset.y -= 0.5f;
	wallB.space->dialate(25.0f);
	wallB.space->translate(ImmersiveEngine::Math::Vector3(18.5f, 10.0f, -28.0f));

	auto wallMesh3 = std::make_shared<Mesh>(Mesh::generateCube(1));
	ImmersiveEngine::cbs::Present wallC("Facade_3", wallMesh3);
	wallC.mesh->setTexture(facadeHTex);
	wallC.mesh->textureOffset.y -= 0.5f;
	wallC.mesh->textureOffset.x -= 0.5f;
	wallC.space->dialate(20.0f);
	wallC.space->translate(ImmersiveEngine::Math::Vector3(-16.0f, 8.0f, -25.0f));

	auto wallMesh4 = std::make_shared<Mesh>(Mesh::generateCube(10));
	ImmersiveEngine::cbs::Present wallD("Facade_4", wallMesh4);
	wallD.mesh->setTexture(facadeCTex);
	wallD.mesh->textureScale = 0.25f;
	wallD.mesh->textureOffset.y -= 0.1f;
	wallD.space->dialate(5.0f);
	wallD.space->translate(ImmersiveEngine::Math::Vector3(-30.0f, 2.0f, 32.0f));

	auto wallMesh5 = std::make_shared<Mesh>(Mesh::generateSquare(3));
	ImmersiveEngine::cbs::Present wallE("Facade_5", wallMesh5);
	wallE.mesh->setTexture(facadeCTex);
	wallE.mesh->textureScale = 1.0f;
	wallE.mesh->textureOffset.y -= 0.5f;
	wallE.space->dialate(20.0f);
	wallE.space->translate(ImmersiveEngine::Math::Vector3(7.0f, 5.0f, 40.0f));

	auto wallMesh6 = std::make_shared<Mesh>(Mesh::generateCube(40));
	ImmersiveEngine::cbs::Present wallF("Facade_6", wallMesh6);
	wallF.mesh->setTexture(facadeBTex);
	wallF.mesh->textureScale = 0.1f;
	//wallF.mesh->textureOffset.y -= 0.9f;
	wallF.space->scale.y = 5.0f;
	wallF.space->translate(ImmersiveEngine::Math::Vector3(-40.0f, 30.0f, 50.0f));

	auto wallMesh7 = std::make_shared<Mesh>(Mesh::generateCube(1));
	ImmersiveEngine::cbs::Present wallG("Facade_7", wallMesh7);
	wallG.mesh->setTexture(facadeETex);
	wallG.mesh->textureOffset.y -= 0.5f;
	wallG.mesh->textureOffset.x -= 0.5f;
	wallG.space->dialate(28.0f);
	wallG.space->translate(ImmersiveEngine::Math::Vector3(45.0f, 10.0f, -28.0f));

	auto wallMesh8 = std::make_shared<Mesh>(Mesh::generateCube(1));
	ImmersiveEngine::cbs::Present wallH("Facade_8", wallMesh8);
	wallH.mesh->setTexture(facadeGTex);
	wallH.mesh->textureOffset.y -= 0.3f;
	wallH.mesh->textureOffset.x -= 0.4f;
	wallH.mesh->textureScale = 1.6f;
	wallH.space->dialate(25.0f);
	wallH.space->translate(ImmersiveEngine::Math::Vector3(65.0f, 10.0f, -2.0f));

	auto wallMesh9 = std::make_shared<Mesh>(Mesh::generateSquare(3));
	ImmersiveEngine::cbs::Present wallI("Facade_9", wallMesh9);
	wallI.mesh->setTexture(facadeFTex);
	wallI.mesh->textureScale = 0.5f;
	wallI.mesh->textureOffset.x -= 0.7f;
	wallI.mesh->textureOffset.y -= 0.7f;
	wallI.space->dialate(5.0f);
	wallI.space->translate(ImmersiveEngine::Math::Vector3(55.0f, 1.0f, 18.0f));
	wallI.space->rotate(90.0f, ImmersiveEngine::Math::Vector3::up);

	auto wallMesh10 = std::make_shared<Mesh>(Mesh::generateCube(1));
	ImmersiveEngine::cbs::Present wallJ("Facade_10", wallMesh10);
	wallJ.mesh->setTexture(facadeITex);
	wallJ.mesh->textureOffset.y -= 0.7f;
	wallJ.mesh->textureOffset.x -= 0.5f;
	wallJ.mesh->textureScale = 1.3f;
	wallJ.space->dialate(30.0f);
	wallJ.space->translate(ImmersiveEngine::Math::Vector3(68.0f, 4.7f, 40.5f));

	auto handMesh = std::make_shared<Mesh>(Mesh::generateSphere(0.06f, 8, 8));

	ImmersiveEngine::cbs::Present lightA;
	lightA.space->translate(ImmersiveEngine::Math::Vector3(-10.0f, 3.0f, 5.0f));
	ImmersiveEngine::cbs::Light* lightCompA = lightA.addComponent<ImmersiveEngine::cbs::Light>(ImmersiveEngine::Math::Vector3(255, 255, 255), 1.0f);
	lightCompA->diffuse.color = ImmersiveEngine::Math::Vector3(250, 200, 90);
	lightCompA->diffuse.intensity = 3.5f;
	lightCompA->specular.color = ImmersiveEngine::Math::Vector3(250, 150, 90);
	ImmersiveEngine::cbs::LightingManager::getInstance().addLight(*lightCompA);
	
	ImmersiveEngine::cbs::Present lightB;
	lightB.space->translate(ImmersiveEngine::Math::Vector3(10.0f, 3.0f, 5.0f));
	ImmersiveEngine::cbs::Light* lightCompB = lightB.addComponent<ImmersiveEngine::cbs::Light>(ImmersiveEngine::Math::Vector3(255, 255, 255), 1.0f);
	lightCompB->diffuse.color = ImmersiveEngine::Math::Vector3(250, 200, 90);
	lightCompB->diffuse.intensity = 3.5f;
	lightCompB->specular.color = ImmersiveEngine::Math::Vector3(250, 150, 90);
	ImmersiveEngine::cbs::LightingManager::getInstance().addLight(*lightCompB);

	ImmersiveEngine::cbs::Present lightC;
	lightC.space->translate(ImmersiveEngine::Math::Vector3(40.0f, 3.0f, 5.0f));
	ImmersiveEngine::cbs::Light* lightCompC = lightC.addComponent<ImmersiveEngine::cbs::Light>(ImmersiveEngine::Math::Vector3(255, 255, 255), 1.0f);
	lightCompC->diffuse.color = ImmersiveEngine::Math::Vector3(250, 200, 90);
	lightCompC->diffuse.intensity = 3.5f;
	lightCompC->specular.color = ImmersiveEngine::Math::Vector3(250, 150, 90);
	ImmersiveEngine::cbs::LightingManager::getInstance().addLight(*lightCompC);

	ImmersiveEngine::cbs::Present lightD;
	lightD.space->translate(ImmersiveEngine::Math::Vector3(40.0f, 3.0f, 30.0f));
	ImmersiveEngine::cbs::Light* lightCompD = lightD.addComponent<ImmersiveEngine::cbs::Light>(ImmersiveEngine::Math::Vector3(255, 255, 255), 1.0f);
	lightCompD->diffuse.color = ImmersiveEngine::Math::Vector3(250, 200, 90);
	lightCompD->diffuse.intensity = 3.5f;
	lightCompD->specular.color = ImmersiveEngine::Math::Vector3(250, 150, 90);
	ImmersiveEngine::cbs::LightingManager::getInstance().addLight(*lightCompD);

	//ImmersiveEngine::cbs::LightingManager::getInstance().useGlobalLight = false;

	ImmersiveEngine::cbs::Present leftHand("Left Hand", handMesh);
	ImmersiveEngine::cbs::Present rightHand("Right Hand", handMesh);
	
	float camWalkSpeed = 10.0f;
	float camSprintSpeed = 30.0f;
	float camSpeed = 0;

	double mouseX = ImmersiveEngine::Settings::g_screenWidth / 2;
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

	float x1 = 0;
	float x2 = 0;

	float offset = 0;
	while (!glfwWindowShouldClose(window))
	{
		x2 = glfwGetTime();
		float deltaTime = x2 - x1;
		x1 = x2;

		if (openInVR)
		{
			xr.pollEvents();
		}

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
		if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		{
			std::cout << xr.toString();
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			clickIn = true;
			glfwSetCursorPos(window, (double)(ImmersiveEngine::Settings::g_screenWidth / 2), (double)(ImmersiveEngine::Settings::g_screenHeight / 2));
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
		//cam.space->position.y = 0.5f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			cam.space->translate(-cam.space->getForward() * deltaTime * camSpeed);
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			cam.space->translate(cam.space->getForward() * camSpeed * deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			cam.space->translate(cam.space->getRight() * camSpeed * deltaTime);
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			cam.space->translate(-cam.space->getRight() * camSpeed * deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			cam.space->translate(ImmersiveEngine::Math::Vector3::up * camSpeed * deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			camSpeed = camSprintSpeed;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			camSpeed = camWalkSpeed;
		}

		if (openInVR && xr.sessionRunning)
		{
			//std::cout << "Session is running.\n";
			xr.waitFrame();
			xr.beginFrame();
			
			xr.input.syncInputs();
			auto poseL = xr.input.getPoseValue(InputPath::leftGripPose);
			leftHand.space->position = poseL.position + cam.space->position;
			leftHand.space->orientation = poseL.orientation;

			auto poseR = xr.input.getPoseValue(InputPath::rightGripPose);
			rightHand.space->position = poseR.position + cam.space->position;
			rightHand.space->orientation = poseR.orientation;

			for (uint32_t i = 0; i < xr.getEyeCount(); ++i)
			{
				GLuint colorImage = xr.getFrameColorImage(i);
				GLuint depthImage = xr.getFrameDepthImage(i);

				xr.waitRenderToEye(i);

				XrViewConfigurationView viewConfig = xr.getViewConfig(i);
				XrView view = xr.getView(i);

				eyeFBO[i].Bind();
				eyeFBO[i].AttachExternalTexture(GL_COLOR_ATTACHMENT0, colorImage, viewConfig.recommendedImageRectWidth, viewConfig.recommendedImageRectHeight);
				eyeFBO[i].AttachExternalTexture(GL_DEPTH_ATTACHMENT, depthImage, viewConfig.recommendedImageRectWidth, viewConfig.recommendedImageRectHeight);

				glClearColor(0.0f, 0.5f, 0.4f, 1.0f); // Window background in decimal RGBA
				glClearDepth(1.0f);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glViewport(0, 0, viewConfig.recommendedImageRectWidth, viewConfig.recommendedImageRectHeight);

				shaderProgram.Activate();
				camComp->refreshViewProjection(shaderProgram, view);



				leftHand.space->refreshTransforms(shaderProgram);
				leftHand.mesh->draw(shaderProgram);

				rightHand.space->refreshTransforms(shaderProgram);
				rightHand.mesh->draw(shaderProgram);

				eyeFBO[i].Unbind();

				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glViewport(0, 0, viewConfig.recommendedImageRectWidth, viewConfig.recommendedImageRectHeight);

				screenShader.Activate();
				eyeFBO[i].DrawScreen();
				glFlush();

				xr.endRenderToEye(i);
			}
			xr.endFrame();
		}
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		FBO.Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Window background in decimal RGBA
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);

		FBO.Resize(width, height);

		shaderProgram.Activate();

		camComp->refreshViewProjection(shaderProgram, (float)width / height);

		ImmersiveEngine::cbs::LightingManager::getInstance().refreshLights(shaderProgram);

		cam.space->refreshTransforms(shaderProgram);

		planeA.space->refreshTransforms(shaderProgram);
		planeA.mesh->draw(shaderProgram);

		planeB.space->refreshTransforms(shaderProgram);
		planeB.mesh->draw(shaderProgram);

		planeC.space->refreshTransforms(shaderProgram);
		planeC.mesh->draw(shaderProgram);

		wallA.space->refreshTransforms(shaderProgram);
		wallA.mesh->draw(shaderProgram);

		wallB.space->refreshTransforms(shaderProgram);
		wallB.mesh->draw(shaderProgram);

		wallC.space->refreshTransforms(shaderProgram);
		wallC.mesh->draw(shaderProgram);

		wallD.space->refreshTransforms(shaderProgram);
		wallD.mesh->draw(shaderProgram);

		wallE.space->refreshTransforms(shaderProgram);
		wallE.mesh->draw(shaderProgram);

		wallF.space->refreshTransforms(shaderProgram);
		wallF.mesh->draw(shaderProgram);

		wallG.space->refreshTransforms(shaderProgram);
		wallG.mesh->draw(shaderProgram);

		wallH.space->refreshTransforms(shaderProgram);
		wallH.mesh->draw(shaderProgram);

		wallI.space->refreshTransforms(shaderProgram);
		wallI.mesh->draw(shaderProgram);

		wallJ.space->refreshTransforms(shaderProgram);
		wallJ.mesh->draw(shaderProgram);

		FBO.Unbind();

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, width, height);

		screenShader.Activate();
		FBO.DrawScreen();

		glfwSwapBuffers(window); // Wait until next frame is rendered before switching to it.
		glfwPollEvents(); // Process window events.
	}
	FBO.Delete();

	shaderProgram.Delete();
	screenShader.Delete();

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