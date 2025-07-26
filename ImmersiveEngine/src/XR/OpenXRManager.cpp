#include"OpenXRManager.h"

namespace ImmersiveEngine::XR
{
	OpenXRManager::OpenXRManager() { }

	OpenXRManager::~OpenXRManager()
	{
		for (uint32_t i = 0; i < m_views.size(); ++i)
		{
			utils::destroySwapchain(m_swapchains[i]);
		}
		utils::destroySession(m_session);
		utils::destroyInstance(m_instance);
		m_swapchains.clear();
		m_images.clear();
		m_views.clear();
	}

	/// Set up a connection with the headset.
	void OpenXRManager::establishConnection()
	{
		XrApplicationInfo app = utils::generateApplicationInfo("HelloWorld", 1, "ImmersiveEngine", 1);
		XrResult instanceCreated = utils::createInstance(app, m_enabledExtensions, &m_instance);
		if (instanceCreated != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not create instance.\n";
			return;
		}
		XrResult gotSystemID = utils::getXRSystemID(m_instance, &m_connectedSystemID);
		if (gotSystemID != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not get system ID.\n";
			return;
		}
		XrResult gotSystemProperties = utils::getXRSystemProperties(m_instance, m_connectedSystemID, &m_connectedSystemProperties);
		if (gotSystemProperties != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not get system properties.\n";
			return;
		}

		XrGraphicsRequirementsOpenGLKHR graphicsRequirements = { XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR };
		XrResult gotGraphicsReq = utils::getGraphicsRequirements(m_instance, m_connectedSystemID, &graphicsRequirements);
		if (gotGraphicsReq != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not get graphics requirements: " << gotGraphicsReq << "\n";
			return;
		}

		XrResult sessionCreated = utils::createSession(m_instance, m_connectedSystemID, &m_session);
		if (sessionCreated != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not create session.\n";
			return;
		}

		XrResult createdSpace = utils::createReferenceSpace(m_referenceSpaceType, m_session, &m_referenceSpace);
		if (createdSpace != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not create a reference space.\n";
			return;
		}
		
		XrResult gotViewConfigs = utils::getViewConfigurationViews(m_viewType, m_instance, m_connectedSystemID, &m_viewConfigs);
		if (gotViewConfigs != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not get configuration views.\n";
			return;
		}

		m_swapchains.resize(m_viewConfigs.size());
		m_images.resize(m_viewConfigs.size());
		for (uint32_t i = 0; i < m_swapchains.size(); ++i)
		{
			XrResult createdSwapchains = utils::createSwapchain(m_viewConfigs[i], m_session, &m_swapchains[i]);
			if (createdSwapchains != XR_SUCCESS)
			{
				std::cerr << "XR_INIT_ERROR could not create swapchain.\n";
				return;
			}
			XrResult gotImages = utils::enumerateSwapchainImages(m_swapchains[i], &m_images[i]);
			if (gotImages != XR_SUCCESS)
			{
				std::cerr << "XR_INIT_ERROR could not acquire swapchain images.\n" << gotImages;
				return;
			}
		}
		std::cout << "[OPENXR] Established connection!";
	}

	/// Read the event queue of the runtime for the instance and session. React to these changes accordingly.
	void OpenXRManager::pollEvents()
	{
		if (m_instance == XR_NULL_HANDLE) return;
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

	void OpenXRManager::waitRenderToEye(uint32_t eyeIndex)
	{
		XrResult waited = utils::waitSwapchainImage(m_swapchains[eyeIndex]);
		if (waited != XR_SUCCESS)
		{
			std::cerr << "XR_RUNTIME_ERROR could not wait to write to swapchain on eye " << eyeIndex << ".\n";
		}
	}

	void OpenXRManager::endRenderToEye(uint32_t eyeIndex)
	{
		if (m_swapchains.empty()) return;

		XrResult imageReleased = utils::releaseSwapchainImage(m_swapchains[eyeIndex]);
		if (imageReleased != XR_SUCCESS)
		{
			std::cerr << "XR_RUNTIME_ERROR could not release swapchain image on eye " << eyeIndex << ".\n";
			return;
		}
	}

	void OpenXRManager::waitFrame()
	{
		if (m_session == XR_NULL_HANDLE) return;
		XrFrameWaitInfo info = { XR_TYPE_FRAME_WAIT_INFO };
		xrWaitFrame(m_session, &info, &m_frameState);
	}

	void OpenXRManager::beginFrame()
	{
		if (m_session == XR_NULL_HANDLE) return;
		XrFrameBeginInfo info = { XR_TYPE_FRAME_BEGIN_INFO };
		xrBeginFrame(m_session, &info);
		
		uint32_t viewCount = getEyeCount();

		XrResult gotViews = utils::getViews(m_viewType, m_frameState, m_referenceSpace, m_session, viewCount, &m_views);
		if (gotViews != XR_SUCCESS)
		{
			std::cerr << "XR_RUNTIME_ERROR could not locate views.\n" << gotViews;
		}
	}

	void OpenXRManager::endFrame()
	{
		if (m_session == XR_NULL_HANDLE) return;

		std::vector<XrCompositionLayerProjectionView> projectionViews = { };
		for (uint32_t i = 0; i < getEyeCount(); ++i)
		{
			projectionViews[i] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
			projectionViews[i].pose = m_views[i].pose;
			projectionViews[i].fov = m_views[i].fov;
			projectionViews[i].subImage.swapchain = m_swapchains[i];
			projectionViews[i].subImage.imageRect.offset = { 0, 0 };
			projectionViews[i].subImage.imageRect.extent = {
				(int32_t)m_viewConfigs[i].recommendedImageRectWidth,
				(int32_t)m_viewConfigs[i].recommendedImageRectHeight
			};
		}
		XrCompositionLayerProjection layer = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
		layer.space = m_referenceSpace;
		layer.viewCount = (uint32_t)projectionViews.size();
		layer.views = projectionViews.data();

		XrCompositionLayerBaseHeader* layers[] = { (XrCompositionLayerBaseHeader*)&layer };

		XrFrameEndInfo info = { XR_TYPE_FRAME_END_INFO };
		info.displayTime = m_frameState.predictedDisplayTime;
		info.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
		info.layerCount = 1;
		info.layers = layers;

		xrEndFrame(m_session, &info);
	}

	uint32_t OpenXRManager::getEyeCount()
	{
		if (m_connectedSystemID == XR_NULL_SYSTEM_ID || m_views.empty()) return 0;
		return m_views.size();
	}
	XrViewConfigurationView OpenXRManager::getViewConfig(uint32_t eyeIndex)
	{
		if (m_viewConfigs.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get view configurations, does not exist.\n";
			return XrViewConfigurationView();
		}
		if (eyeIndex >= m_viewConfigs.size())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get view config, eye index out of bounds.\n";
			return XrViewConfigurationView();
		}
		return m_viewConfigs[eyeIndex];
	}
	XrView OpenXRManager::getView(uint32_t eyeIndex)
	{
		if (m_viewConfigs.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get view, does not exist.\n";
			return XrView();
		}
		if (eyeIndex >= m_viewConfigs.size())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get view, eye index out of bounds.\n";
			return XrView();
		}
		return m_views[eyeIndex];
	}
	uint32_t OpenXRManager::getFrameImage(uint32_t eyeIndex)
	{
		if (m_swapchains.empty() || m_images.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get frame image, initialized incorrectly.\n";
			return 0;
		}
		if (eyeIndex >= m_views.size())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get frame image, eye index out of bounds.\n";
			return 0;
		}

		uint32_t imageIndex;
		XrResult imageIndexAcquired = utils::acquireSwapchainImage(m_swapchains[eyeIndex], &imageIndex);
		if (imageIndexAcquired != XR_SUCCESS)
		{
			std::cerr << "XR_RUNTIME_ERROR could not acquire swapchain image index on eye " << eyeIndex << ".\n";
			return 0;
		}

		return m_images[eyeIndex][imageIndex].image; // Get image by current eye (row) and next available image (column).
	}

	std::string OpenXRManager::toString()
	{
		std::ostringstream oss;
		oss << "[OPENXR MANAGER]" << "\nDevice: " << m_connectedSystemProperties.systemName << "\nCurrent State: " << m_currentSessionState.second << "\n";
		return oss.str();
	}
}