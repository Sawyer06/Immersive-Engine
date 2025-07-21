#include"OpenXRManager.h"

namespace ImmersiveEngine::XR
{
	OpenXRManager::OpenXRManager() { }

	OpenXRManager::~OpenXRManager()
	{
		destroySession(m_session);
		destroyInstance(m_instance);
		for (uint32_t i = 0; i < m_views.size(); ++i)
		{
			destroySwapchain(m_swapchains[i]);
		}
		m_swapchains.clear();
		m_images.clear();
		m_views.clear();
	}

	/// Set up a connection with the headset.
	void OpenXRManager::establishConnection()
	{
		XrApplicationInfo app = generateApplicationInfo("HelloWorld", 1, "ImmersiveEngine", 1);
		XrResult instanceCreated = createInstance(app, &m_instance);
		if (instanceCreated == XR_SUCCESS)
		{
			XrResult gotSystemID = getXRSystemID(m_instance, &m_connectedSystemID);
			if (gotSystemID == XR_SUCCESS)
			{
				XrResult gotSystemProperties = getXRSystemProperties(m_instance, m_connectedSystemID, &m_connectedSystemProperties);
				if (gotSystemProperties != XR_SUCCESS)
				{
					std::cerr << "XR_INIT_ERROR could not get system properties.\n";
				}
				std::cout << "Device: " << m_connectedSystemProperties.systemName << "\n";
				
				XrResult sessionCreated = createSession(m_instance, m_connectedSystemID, &m_session); // ERROR: returns -38, graphics device invalid.
				std::cout << "SESSION RESULT: " << sessionCreated << "\n";
				if (sessionCreated == XR_SUCCESS)
				{
					XrResult gotViews = getViewConfigurationViews(m_viewType, m_instance, m_connectedSystemID, &m_views);
					if (gotViews == XR_SUCCESS)
					{
						m_swapchains.resize(m_views.size());
						for (uint32_t i = 0; i < m_swapchains.size(); ++i)
						{
							XrResult createdSwapchains = createSwapchain(m_views[i], m_session, &m_swapchains[i]);
							if (createdSwapchains == XR_SUCCESS)
							{
								XrResult gotImages = enumerateSwapchainImages(m_swapchains[i], &m_images[i]);
								if (gotImages != XR_SUCCESS)
								{
									std::cerr << "XR_INIT_ERROR could acquire swapchain images.\n";
								}
							}
							else
							{
								std::cerr << "XR_INIT_ERROR could not create swapchain.\n";
							}
						}
					}
					else
					{
						std::cerr << "XR_INIT_ERROR could not get configuration views.\n";
					}
				}
				else
				{
					std::cerr << "XR_INIT_ERROR could not create session.\n";
				}
			}
			else
			{
				std::cerr << "XR_INIT_ERROR could not get system ID.\n";
			}
		}
		else
		{
			std::cerr << "XR_INIT_ERROR could not create instance.\n";
		}
	}

	/// Read the event queue of the runtime for the instance and session. React to these changes accordingly.
	void OpenXRManager::pollEvents()
	{
		XrEventDataBuffer event = { XR_TYPE_EVENT_DATA_BUFFER };
		XrResult polling = xrPollEvent(m_instance, &event);
		if (polling == XR_SUCCESS)
		{
			if (event.type == XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED) // The state of the session has changed.
			{
				auto* sessionStateChangedEvent = (XrEventDataSessionStateChanged*)&event;
				switch (sessionStateChangedEvent->state)
				{
					case XR_SESSION_STATE_IDLE:
					{
						m_currentSessionState = {XR_SESSION_STATE_IDLE, "IDLE"};
						break;
					}
					case XR_SESSION_STATE_READY:
					{
						m_currentSessionState = { XR_SESSION_STATE_READY, "READY" };
						XrSessionBeginInfo info = { XR_TYPE_SESSION_BEGIN_INFO };
						info.primaryViewConfigurationType = m_viewType; // Two views
						xrBeginSession(m_session, &info);
						sessionRunning = true;
						break;
					}
					case XR_SESSION_STATE_SYNCHRONIZED:
					{
						m_currentSessionState = { XR_SESSION_STATE_SYNCHRONIZED, "SYNCHRONIZED" };
						break;
					}
					case XR_SESSION_STATE_VISIBLE:
					{
						m_currentSessionState = { XR_SESSION_STATE_VISIBLE, "VISIBLE" };
						break;
					}
					case XR_SESSION_STATE_FOCUSED:
					{
						m_currentSessionState = { XR_SESSION_STATE_FOCUSED, "FOCUSED" };
						break;
					}
					case XR_SESSION_STATE_STOPPING:
					{
						m_currentSessionState = { XR_SESSION_STATE_STOPPING, "STOPPING" };
						xrEndSession(m_session);
						sessionRunning = false;
						break;
					}
					case XR_SESSION_STATE_LOSS_PENDING:
					{
						m_currentSessionState = { XR_SESSION_STATE_LOSS_PENDING, "STATE LOSS PENDING" };

						sessionRunning = false;
						break;
					}
					case XR_SESSION_STATE_EXITING:
					{
						m_currentSessionState = { XR_SESSION_STATE_EXITING, "EXITING" };
						sessionRunning = false;
						break;
					}
					default:
					{
						m_currentSessionState = { XR_SESSION_STATE_UNKNOWN, "UNKNOWN" };
						break;
					}
				}
			}
			else if (event.type == XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING) // Application is about to lose the instance.
			{
				auto* instanceLossPendingEvent = (XrEventDataInstanceLossPending*)&event;
				sessionRunning = false;
				std::cerr << "XR_RUNTIME_ERROR instance loss pending at: " << instanceLossPendingEvent->lossTime << "\n";
			}
		}
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

	XrApplicationInfo OpenXRManager::generateApplicationInfo(std::string appName, uint32_t appVersion, std::string engineName, uint32_t engineVersion)
	{
		XrApplicationInfo app;
		strncpy(app.applicationName, appName.c_str(), XR_MAX_APPLICATION_NAME_SIZE);
		app.applicationVersion = appVersion;
		strncpy(app.engineName, engineName.c_str(), XR_MAX_ENGINE_NAME_SIZE);
		app.engineVersion = engineVersion;
		app.apiVersion = XR_CURRENT_API_VERSION;

		return app;
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
	XrResult OpenXRManager::getViewConfigurationViews(XrViewConfigurationType& viewType, XrInstance& instance, XrSystemId& systemID, std::vector<XrViewConfigurationView>* o_views)
	{
		uint32_t viewCount = 0;
		xrEnumerateViewConfigurationViews(instance, systemID, viewType, 0, &viewCount, nullptr); // First get number of views.
		o_views->resize(viewCount);

		return xrEnumerateViewConfigurationViews(instance, systemID, viewType, viewCount, &viewCount, o_views->data()); // Populate views list.
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

	/// Create a swapchain (a reel of buffers) for a specific eye view.
	XrResult OpenXRManager::createSwapchain(XrViewConfigurationView& view, XrSession& session, XrSwapchain* o_swapchain)
	{
		XrSwapchainCreateInfo info = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
		info.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
		info.format = GL_SRGB8_ALPHA8;
		info.sampleCount = view.recommendedSwapchainSampleCount;
		info.width = view.recommendedImageRectWidth;
		info.height = view.maxImageRectHeight;
		info.faceCount = 1;
		info.arraySize = 1;
		info.mipCount = 1;

		return xrCreateSwapchain(session, &info, o_swapchain);
	}
	
	XrResult OpenXRManager::acquireSwapchainImage(XrSwapchain& swapchain, uint32_t* o_index)
	{
		XrSwapchainImageAcquireInfo info = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
		return xrAcquireSwapchainImage(swapchain, &info, o_index);
	}

	/// Gets the image list for a swapchain.
	XrResult OpenXRManager::enumerateSwapchainImages(XrSwapchain& swapchain, std::vector<XrSwapchainImageOpenGLKHR>* o_images)
	{
		uint32_t imageCount = 0;
		xrEnumerateSwapchainImages(swapchain, 0, &imageCount, nullptr);
		o_images->resize(imageCount);
		return xrEnumerateSwapchainImages(swapchain, imageCount, &imageCount, (XrSwapchainImageBaseHeader*)o_images->data());
	}

	/// Wait on oldest acquired swapchain image to avoid writing to it before the compositer has finished reading it.
	XrResult OpenXRManager::waitSwapchainImage(XrSwapchain& swapchain)
	{
		XrSwapchainImageWaitInfo info = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
		return xrWaitSwapchainImage(swapchain, &info);
	}

	/// Give the oldest acquired swapchain image back to the swapchain for reuse.
	XrResult OpenXRManager::releaseSwapchainImage(XrSwapchain& swapchain)
	{
		XrSwapchainImageReleaseInfo info = { XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
		return xrReleaseSwapchainImage(swapchain, &info);
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
	XrResult OpenXRManager::destroySwapchain(XrSwapchain& swapchain)
	{
		if (swapchain == XR_NULL_HANDLE) return XR_ERROR_RUNTIME_FAILURE;
		return xrDestroySwapchain(swapchain);
	}

	uint32_t OpenXRManager::getEyeCount()
	{
		if (m_instance == XR_NULL_HANDLE) return 0;
		return m_views.size();
	}

	std::string OpenXRManager::toString()
	{
		std::ostringstream oss;
		oss << "[OPENXR MANAGER]";
		return oss.str();
	}
}