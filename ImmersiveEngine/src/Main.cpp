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

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

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
#include"Rendering/Cubemap.h"

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

static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	std::cout << buffer << std::endl;
}

#ifdef JPH_ENABLE_ASSERTS

static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
{
	std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << std::endl;

	return true;
};

#endif // JPH_ENABLE_ASSERTS

namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
		{
			switch (inObject1)
			{
				case Layers::NON_MOVING:
					return inObject2 == Layers::MOVING;
				case Layers::MOVING:
					return true;
				default:
					return false;
			}
		}
};

namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr uint32_t NUM_LAYERS(2);
}

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
	public:
		BPLayerInterfaceImpl()
		{
			m_objectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
			m_objectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
		}

		virtual uint32_t GetNumBroadPhaseLayers() const override
		{
			return BroadPhaseLayers::NUM_LAYERS;
		}
		virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
		{
			return m_objectToBroadPhase[inLayer];
		}
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
		{
			switch ((JPH::BroadPhaseLayer::Type)inLayer)
			{
				case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	
					return "NON_MOVING";
				case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		
					return "MOVING";
				default:													
					JPH_ASSERT(false); return "INVALID";
			}
		}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED
	private:
		JPH::BroadPhaseLayer m_objectToBroadPhase[Layers::NUM_LAYERS];
};

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
		{
			switch (inLayer1)
			{
				case Layers::NON_MOVING:
					return inLayer2 == BroadPhaseLayers::MOVING;
				case Layers::MOVING:
					return true;
				default:
					return false;
			}
		}
};

class ContactListenerImpl : public JPH::ContactListener
{
	public:
		virtual JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollectionResult) override
		{
			return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
		}
		virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
		{
			std::cout << "A contact was added" << std::endl;
		}
		virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
		{
			std::cout << "A contact was persisted" << std::endl;
		}

		virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
		{
			std::cout << "A contact was removed" << std::endl;
		}

};

class BodyActivationListenerImpl : public JPH::BodyActivationListener
{
	virtual void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
	{
		std::cout << "A body got activated" << std::endl;
	}

	virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
	{
		std::cout << "A body went to sleep" << std::endl;
	}
};

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
	glfwSwapInterval(0); // vsync

	bool openInVR = false;
	std::vector<ImmersiveEngine::Rendering::FBO> eyeFBO;
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
	glFrontFace(GL_CCW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ImmersiveEngine::Rendering::Shader shaderProgram("default.vert", "default.frag");
	ImmersiveEngine::Rendering::Shader screenShader("screen.vert", "screen.frag");
	ImmersiveEngine::Rendering::Shader skyboxShader("skybox.vert", "skybox.frag");

	ImmersiveEngine::Rendering::Cubemap skybox({
		"skybox-right.png",
		"skybox-left.png",
		"skybox-top.png",
		"skybox-bottom.png",
		"skybox-front.png",
		"skybox-back.png"
	});
	skyboxShader.Activate();
	skyboxShader.setInt("skybox", 0);
	skyboxShader.setFloat("brightness", 0.3f);

	//screenShader.Activate();

	ImmersiveEngine::Rendering::FBO FBO;
	FBO.Resize(ImmersiveEngine::Settings::g_screenWidth, ImmersiveEngine::Settings::g_screenHeight);

	ImmersiveEngine::cbs::Present cam;
	ImmersiveEngine::cbs::Camera* camComp = cam.addComponent<ImmersiveEngine::cbs::Camera>();
	cam.space->position = ImmersiveEngine::Math::Vector3(0, 0, 2);
	//cam.space->position = ImmersiveEngine::Math::Vector3(2.0f, 1.0f, 2);
	//cam.space->rotate(ImmersiveEngine::Math::Vector3(-1.0f, -0.5f, 0));

	std::shared_ptr<ImmersiveEngine::Rendering::Texture> deadGrassTex = std::make_shared<ImmersiveEngine::Rendering::Texture>("dead-grass.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<ImmersiveEngine::Rendering::Texture> cobblestoneTex = std::make_shared<ImmersiveEngine::Rendering::Texture>("cobblestone.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<ImmersiveEngine::Rendering::Texture> facadeATex = std::make_shared<ImmersiveEngine::Rendering::Texture>("abandoned-building1.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<ImmersiveEngine::Rendering::Texture> facadeBTex = std::make_shared<ImmersiveEngine::Rendering::Texture>("abandoned-building2.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<ImmersiveEngine::Rendering::Texture> facadeCTex = std::make_shared<ImmersiveEngine::Rendering::Texture>("abandoned-building3.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<ImmersiveEngine::Rendering::Texture> facadeDTex = std::make_shared<ImmersiveEngine::Rendering::Texture>("abandoned-building4.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<ImmersiveEngine::Rendering::Texture> facadeETex = std::make_shared<ImmersiveEngine::Rendering::Texture>("abandoned-building5.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<ImmersiveEngine::Rendering::Texture> facadeFTex = std::make_shared<ImmersiveEngine::Rendering::Texture>("abandoned-building6.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<ImmersiveEngine::Rendering::Texture> facadeGTex = std::make_shared<ImmersiveEngine::Rendering::Texture>("abandoned-building7.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<ImmersiveEngine::Rendering::Texture> facadeHTex = std::make_shared<ImmersiveEngine::Rendering::Texture>("abandoned-building8.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);
	std::shared_ptr<ImmersiveEngine::Rendering::Texture> facadeITex = std::make_shared<ImmersiveEngine::Rendering::Texture>("abandoned-building9.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);

	auto ballMesh = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateSphere(1, 24, 24));
	ImmersiveEngine::cbs::Present ball("Ball", ballMesh);
	ball.space->dialate(1.0f);
	ball.space->translate(ImmersiveEngine::Math::Vector3(20.0f, 100.0f, -10.0f));

	auto planeMesh1 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generatePlane(15, 4));
	ImmersiveEngine::cbs::Present planeA("Plane_1", planeMesh1);
	planeA.mesh->setTexture(cobblestoneTex);
	planeA.mesh->textureScale = 1.2f;
	planeA.space->dialate(5.0f);
	planeA.space->translate(ImmersiveEngine::Math::Vector3(0.0f, -2.0f, -10.0f));
	ImmersiveEngine::cbs::Present planeB("Plane_2", planeMesh1);
	planeB.space->dialate(5.0f);
	planeB.space->translate(ImmersiveEngine::Math::Vector3(47.5f, -2.0f, 10.0f));
	planeB.space->rotate(90.0f, ImmersiveEngine::Math::Vector3::up);

	auto planeMesh2 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generatePlane(15, 10));
	ImmersiveEngine::cbs::Present planeC("Plane_3", planeMesh2);
	planeC.mesh->setTexture(deadGrassTex);
	planeC.mesh->textureScale = 0.6f;
	planeC.space->dialate(5.0f);
	planeC.space->translate(ImmersiveEngine::Math::Vector3(0.0f, -2.0f, 25.0f));

	auto wallMesh1 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateSquare(1));
	ImmersiveEngine::cbs::Present wallA("Facade_1", wallMesh1);
	wallA.mesh->setTexture(facadeATex);
	wallA.mesh->textureScale = 1.0f;
	wallA.mesh->textureOffset.y -= 0.5f;
	wallA.space->dialate(12.0f);
	wallA.space->translate(ImmersiveEngine::Math::Vector3(0.0f, 4.0f, -16.0f));

	auto wallMesh2 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateCube(1));
	ImmersiveEngine::cbs::Present wallB("Facade_2", wallMesh2);
	wallB.mesh->setTexture(facadeDTex);
	wallB.mesh->textureOffset.y -= 0.5f;
	wallB.space->dialate(25.0f);
	wallB.space->translate(ImmersiveEngine::Math::Vector3(18.5f, 10.0f, -28.0f));

	auto wallMesh3 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateCube(1));
	ImmersiveEngine::cbs::Present wallC("Facade_3", wallMesh3);
	wallC.mesh->setTexture(facadeHTex);
	wallC.mesh->textureOffset.y -= 0.5f;
	wallC.mesh->textureOffset.x -= 0.5f;
	wallC.space->dialate(20.0f);
	wallC.space->translate(ImmersiveEngine::Math::Vector3(-16.0f, 8.0f, -25.0f));

	auto wallMesh4 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateCube(10));
	ImmersiveEngine::cbs::Present wallD("Facade_4", wallMesh4);
	wallD.mesh->setTexture(facadeCTex);
	wallD.mesh->textureScale = 0.25f;
	wallD.mesh->textureOffset.y -= 0.1f;
	wallD.space->dialate(5.0f);
	wallD.space->translate(ImmersiveEngine::Math::Vector3(-30.0f, 2.0f, 32.0f));

	auto wallMesh5 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateSquare(3));
	ImmersiveEngine::cbs::Present wallE("Facade_5", wallMesh5);
	wallE.mesh->setTexture(facadeCTex);
	wallE.mesh->textureScale = 1.0f;
	wallE.mesh->textureOffset.y -= 0.5f;
	wallE.space->dialate(20.0f);
	wallE.space->translate(ImmersiveEngine::Math::Vector3(7.0f, 5.0f, 40.0f));
	wallE.space->rotate(180, ImmersiveEngine::Math::Vector3::up);

	auto wallMesh6 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateCube(40));
	ImmersiveEngine::cbs::Present wallF("Facade_6", wallMesh6);
	wallF.mesh->setTexture(facadeBTex);
	wallF.mesh->textureScale = 0.1f;
	//wallF.mesh->textureOffset.y -= 0.9f;
	wallF.space->scale.y = 5.0f;
	wallF.space->translate(ImmersiveEngine::Math::Vector3(-40.0f, 30.0f, 50.0f));

	auto wallMesh7 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateCube(1));
	ImmersiveEngine::cbs::Present wallG("Facade_7", wallMesh7);
	wallG.mesh->setTexture(facadeETex);
	wallG.mesh->textureOffset.y -= 0.5f;
	wallG.mesh->textureOffset.x -= 0.5f;
	wallG.space->dialate(28.0f);
	wallG.space->translate(ImmersiveEngine::Math::Vector3(45.0f, 10.0f, -28.0f));

	auto wallMesh8 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateCube(1));
	ImmersiveEngine::cbs::Present wallH("Facade_8", wallMesh8);
	wallH.mesh->setTexture(facadeGTex);
	wallH.mesh->textureOffset.y -= 0.3f;
	wallH.mesh->textureOffset.x -= 0.4f;
	wallH.mesh->textureScale = 1.6f;
	wallH.space->dialate(25.0f);
	wallH.space->translate(ImmersiveEngine::Math::Vector3(65.0f, 10.0f, -2.0f));

	auto wallMesh9 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateSquare(3));
	ImmersiveEngine::cbs::Present wallI("Facade_9", wallMesh9);
	wallI.mesh->setTexture(facadeFTex);
	wallI.mesh->textureScale = 0.5f;
	wallI.mesh->textureOffset.x -= 0.7f;
	wallI.mesh->textureOffset.y -= 0.7f;
	wallI.space->dialate(5.0f);
	wallI.space->translate(ImmersiveEngine::Math::Vector3(55.0f, 1.0f, 18.0f));
	wallI.space->rotate(-90.0f, ImmersiveEngine::Math::Vector3::up);

	auto wallMesh10 = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateCube(1));
	ImmersiveEngine::cbs::Present wallJ("Facade_10", wallMesh10);
	wallJ.mesh->setTexture(facadeITex);
	wallJ.mesh->textureOffset.y -= 0.7f;
	wallJ.mesh->textureOffset.x -= 0.5f;
	wallJ.mesh->textureScale = 1.3f;
	wallJ.space->dialate(30.0f);
	wallJ.space->translate(ImmersiveEngine::Math::Vector3(68.0f, 4.7f, 40.5f));

	auto handMesh = std::make_shared<ImmersiveEngine::Rendering::Mesh>(ImmersiveEngine::Rendering::Mesh::generateSphere(0.06f, 8, 8));

	ImmersiveEngine::cbs::Present lightA;
	lightA.space->translate(ImmersiveEngine::Math::Vector3(-10.0f, 3.0f, 0.0f));
	ImmersiveEngine::cbs::Light* lightCompA = lightA.addComponent<ImmersiveEngine::cbs::Light>(ImmersiveEngine::Math::Vector3(255, 255, 255), 1.0f);
	lightCompA->diffuse.color = ImmersiveEngine::Math::Vector3(250, 200, 90);
	lightCompA->diffuse.intensity = 2.0f;
	lightCompA->specular.color = ImmersiveEngine::Math::Vector3(250, 150, 90);
	ImmersiveEngine::cbs::LightingManager::getInstance().addLight(*lightCompA);
	
	ImmersiveEngine::cbs::Present lightB;
	lightB.space->translate(ImmersiveEngine::Math::Vector3(10.0f, 3.0f, 0.0f));
	ImmersiveEngine::cbs::Light* lightCompB = lightB.addComponent<ImmersiveEngine::cbs::Light>(ImmersiveEngine::Math::Vector3(255, 255, 255), 1.0f);
	lightCompB->diffuse.color = ImmersiveEngine::Math::Vector3(250, 200, 90);
	lightCompB->diffuse.intensity = 2.0f;
	lightCompB->specular.color = ImmersiveEngine::Math::Vector3(250, 150, 90);
	ImmersiveEngine::cbs::LightingManager::getInstance().addLight(*lightCompB);

	ImmersiveEngine::cbs::Present lightC;
	lightC.space->translate(ImmersiveEngine::Math::Vector3(40.0f, 3.0f, 0.0f));
	ImmersiveEngine::cbs::Light* lightCompC = lightC.addComponent<ImmersiveEngine::cbs::Light>(ImmersiveEngine::Math::Vector3(255, 255, 255), 1.0f);
	lightCompC->diffuse.color = ImmersiveEngine::Math::Vector3(250, 200, 90);
	lightCompC->diffuse.intensity = 2.0f;
	lightCompC->specular.color = ImmersiveEngine::Math::Vector3(250, 150, 90);
	ImmersiveEngine::cbs::LightingManager::getInstance().addLight(*lightCompC);

	ImmersiveEngine::cbs::Present lightD;
	lightD.space->translate(ImmersiveEngine::Math::Vector3(40.0f, 3.0f, 30.0f));
	ImmersiveEngine::cbs::Light* lightCompD = lightD.addComponent<ImmersiveEngine::cbs::Light>(ImmersiveEngine::Math::Vector3(255, 255, 255), 1.0f);
	lightCompD->diffuse.color = ImmersiveEngine::Math::Vector3(250, 200, 90);
	lightCompD->diffuse.intensity = 2.0f;
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

	JPH::RegisterDefaultAllocator();
	JPH::Trace = TraceImpl;
#ifdef JPH_ENABLE_ASSERTS
	JPH::AssertFailed = AssertFailedImpl;
#endif
	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();
	JPH::TempAllocatorImpl tempAllocator(10 * 1024 * 1024);
	JPH::JobSystemThreadPool jobSystem(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);
	
	const uint32_t maxBodies = 1024;
	const uint32_t numBodyMutexes = 0;
	const uint32_t maxBodyPairs = 1024;
	const uint32_t maxContactConstraints = 1024;

	BPLayerInterfaceImpl broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImpl objectVsBroadphaseLayerFilter;
	ObjectLayerPairFilterImpl objectVsObjectLayerFilter;

	JPH::PhysicsSystem physicsSystem;
	physicsSystem.Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints, broadPhaseLayerInterface, objectVsBroadphaseLayerFilter, objectVsObjectLayerFilter);

	BodyActivationListenerImpl bodyActivationListener;
	physicsSystem.SetBodyActivationListener(&bodyActivationListener);

	ContactListenerImpl contactListener;
	physicsSystem.SetContactListener(&contactListener);

	JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

	JPH::BoxShapeSettings floorShapeSettings(JPH::Vec3(100.0f, 1.0f, 100.0f));
	floorShapeSettings.SetEmbedded();

	JPH::ShapeSettings::ShapeResult floorShapeResult = floorShapeSettings.Create();
	JPH::ShapeRefC floorShape = floorShapeResult.Get();

	JPH::BodyCreationSettings floorSettings(floorShape, JPH::RVec3(0.0f, -2.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
	JPH::Body* floor = bodyInterface.CreateBody(floorSettings);
	bodyInterface.AddBody(floor->GetID(), JPH::EActivation::DontActivate);

	JPH::BodyCreationSettings sphereSettings(new JPH::SphereShape(0.5f), JPH::RVec3(ball.space->position.x, ball.space->position.y, ball.space->position.z), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);
	sphereSettings.mRestitution = 0.5f;
	JPH::BodyID sphereID = bodyInterface.CreateAndAddBody(sphereSettings, JPH::EActivation::Activate);

	bodyInterface.SetLinearVelocity(sphereID, JPH::Vec3(0.0f, -5.0f, 2.0f));

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

		if (bodyInterface.IsActive(sphereID))
		{
			JPH::RVec3 position = bodyInterface.GetCenterOfMassPosition(sphereID);
			ball.space->position = ImmersiveEngine::Math::Vector3(position.GetX(), position.GetY(), position.GetZ());

			physicsSystem.Update(deltaTime, 1, &tempAllocator, &jobSystem);
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

				glDepthFunc(GL_LEQUAL);

				skyboxShader.Activate();

				camComp->refreshViewProjection(shaderProgram, skyboxShader, view);

				skybox.draw(skyboxShader);

				glDepthFunc(GL_LESS);

				shaderProgram.Activate();
				camComp->refreshViewProjection(shaderProgram, skyboxShader, view);

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
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);

		FBO.Resize(width, height);

		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();

		camComp->refreshViewProjection(shaderProgram, skyboxShader, (float)width / height);

		skybox.draw(skyboxShader);

		glDepthFunc(GL_LESS);

		shaderProgram.Activate();

		camComp->refreshViewProjection(shaderProgram, skyboxShader, (float)width / height);

		ImmersiveEngine::cbs::LightingManager::getInstance().refreshLights(shaderProgram);

		cam.space->refreshTransforms(shaderProgram);

		ball.space->refreshTransforms(shaderProgram);
		ball.mesh->draw(shaderProgram);

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
	bodyInterface.RemoveBody(sphereID);
	bodyInterface.DestroyBody(sphereID);
	bodyInterface.RemoveBody(floor->GetID());
	bodyInterface.DestroyBody(floor->GetID());

	JPH::UnregisterTypes();
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	FBO.Delete();

	shaderProgram.Delete();
	screenShader.Delete();
	skyboxShader.Delete();

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