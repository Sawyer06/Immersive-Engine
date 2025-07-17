#include"OpenXRManager.h"

namespace ImmersiveEngine::XR
{
	OpenXRManager::OpenXRManager() { }

	OpenXRManager::~OpenXRManager()
	{
		destroySession(m_session);
		destroyInstance(m_instance);
	}

	/// Set up connection with headset.
	void OpenXRManager::establishConnection()
	{
		XrApplicationInfo app = createApplicationInfo("HelloWorld", 1, "ImmersiveEngine", 1);
		XrResult instanceCreated = createInstance(app, &m_instance);
		if (instanceCreated == XR_SUCCESS)
		{
			XrResult gotSystemID = getXRSystemID(m_instance, &m_connectedSystemID);
			if (gotSystemID == XR_SUCCESS)
			{
				XrResult gotSystemProperties = getXRSystemProperties(m_instance, m_connectedSystemID, &m_connectedSystemProperties);
				if (gotSystemProperties != XR_SUCCESS)
				{
					std::cerr << "XR_INIT_ERROR could not get system properties.";
				}
				std::cout << "Device: " << m_connectedSystemProperties.systemName << "\n";
				
				XrResult sessionCreated = createSession(m_instance, m_connectedSystemID, &m_session); // ERROR: returns -38, graphics device invalid.
				std::cout << "SESSION RESULT: " << sessionCreated << "\n";
				if (sessionCreated != XR_SUCCESS)
				{
					std::cerr << "XR_INIT_ERROR could not create session.";
				}
			}
			else
			{
				std::cerr << "XR_INIT_ERROR could not get system ID.";
			}
		}
		else
		{
			std::cerr << "XR_INIT_ERROR could not create instance.";
		}
	}

	void OpenXRManager::pollEvents()
	{

	}

	void OpenXRManager::waitFrame()
	{
		XrFrameWaitInfo info = { XR_TYPE_FRAME_WAIT_INFO };
		XrFrameState frameState = { XR_TYPE_FRAME_STATE };
		xrWaitFrame(m_session, &info, &frameState);
	}

	void OpenXRManager::beginFrame()
	{
		XrFrameBeginInfo info = { XR_TYPE_FRAME_BEGIN_INFO };
		xrBeginFrame(m_session, &info);
	}

	void OpenXRManager::endFrame()
	{
		XrFrameEndInfo info = { XR_TYPE_FRAME_END_INFO };
		xrEndFrame(m_session, &info);
	}

	XrResult OpenXRManager::getXRSystemID(XrInstance& instance, XrSystemId* o_systemID)
	{
		XrSystemGetInfo info { XR_TYPE_SYSTEM_GET_INFO };
		info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		return xrGetSystem(m_instance, &info, o_systemID);
	}
	XrResult OpenXRManager::getXRSystemProperties(XrInstance& instance, XrSystemId& systemID, XrSystemProperties* o_systemProperties)
	{
		return xrGetSystemProperties(instance, systemID, o_systemProperties);
	}


	XrApplicationInfo OpenXRManager::createApplicationInfo(std::string appName, uint32_t appVersion, std::string engineName, uint32_t engineVersion)
	{
		XrApplicationInfo app;
		strncpy(app.applicationName, appName.c_str(), XR_MAX_APPLICATION_NAME_SIZE);
		app.applicationVersion = appVersion;
		strncpy(app.engineName, engineName.c_str(), XR_MAX_ENGINE_NAME_SIZE);
		app.engineVersion = engineVersion;
		app.apiVersion = XR_CURRENT_API_VERSION;
		
		return app;
	}

	/// Create an instance to communicate with the runtime.
	XrResult OpenXRManager::createInstance(XrApplicationInfo& app, XrInstance* o_instance)
	{
		XrInstanceCreateInfo info{ XR_TYPE_INSTANCE_CREATE_INFO };
		info.createFlags = 0;
		info.applicationInfo = app;

		return xrCreateInstance(&info, o_instance);
	}

	/// Create a session to communicate with the connected device.
	XrResult OpenXRManager::createSession(XrInstance& instance, XrSystemId& systemID, XrSession* o_session)
	{
		XrSessionCreateInfo info{ XR_TYPE_SESSION_CREATE_INFO };
		XrGraphicsBindingOpenGLWin32KHR graphicsBinding{ XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR };
		graphicsBinding.hDC = wglGetCurrentDC();
		graphicsBinding.hGLRC = wglGetCurrentContext();

		info.next = &graphicsBinding;
		info.systemId = systemID;

		return xrCreateSession(instance, &info, o_session);
	}
	
	XrResult OpenXRManager::destroyInstance(XrInstance& instance)
	{
		if (instance == XR_NULL_HANDLE) return XR_ERROR_RUNTIME_FAILURE;
		return xrDestroyInstance(instance);
	}
	XrResult OpenXRManager::destroySession(XrSession& session)
	{
		if (session == XR_NULL_HANDLE) return XR_ERROR_RUNTIME_FAILURE;
		return xrDestroySession(session);
	}

	std::string OpenXRManager::toString()
	{
		std::ostringstream oss;
		oss << "[OPENXR MANAGER]";
		return oss.str();
	}
}