#include"OpenXRManager.h"

namespace ImmersiveEngine::XR
{
	OpenXRManager::OpenXRManager() : input(*this)
	{
		m_requestedExtensions.push_back(XR_KHR_OPENGL_ENABLE_EXTENSION_NAME);
	}

	OpenXRManager::~OpenXRManager()
	{
		input.~InputHandler();

		for (uint32_t i = 0; i < m_viewConfigs.size(); ++i)
		{
			utils::destroySwapchain(m_colorSwapchainInfos[i].swapchain);
			utils::destroySwapchain(m_depthSwapchainInfos[i].swapchain);
		}
		utils::destroySession(m_session);
		utils::destroyInstance(m_instance);
		utils::destroySpace(m_referenceSpace);

		m_colorSwapchainInfos.clear();
		m_depthSwapchainInfos.clear();
		m_views.clear();
		m_viewConfigs.clear();
	}

	OpenXRManager::InputHandler::InputHandler(OpenXRManager& manager) : m_manager(manager) { }

	OpenXRManager::InputHandler::~InputHandler()
	{
		for (XrActionSet& set : m_actionSets)
		{
			utils::input::destroyActionSet(set);
		}
		for (utils::input::ActionBinding& actionBinding : m_actionBindings)
		{
			utils::input::destroyAction(actionBinding.action);
		}

		utils::destroySpace(m_leftGripSpace);
		utils::destroySpace(m_rightGripSpace);
		utils::destroySpace(m_leftAimSpace);
		utils::destroySpace(m_rightAimSpace);
	}

	/// Set up a connection with the headset.
	void OpenXRManager::establishConnection()
	{
		XrApplicationInfo app = utils::generateApplicationInfo("HelloWorld", 1, "ImmersiveEngine", 1);

		std::vector<XrApiLayerProperties> apiLayerProperties;
		XrResult apiLayersGotten = utils::getAPILayerProperties(&apiLayerProperties);
		if (apiLayersGotten != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not get API layers from the OpenXR runtime.\n";
			return;
		}

		// Determine active API layers based on what is requested and what the runtime provides.
		for (auto& requestedLayer : m_requestedAPILayers)
		{
			for (auto& layerProperty : apiLayerProperties)
			{
				// If the requested layer and the current gotten layer do not match, check next gotten layer.
				if (strcmp(requestedLayer.c_str(), layerProperty.layerName) != 0)
				{
					continue;
				}
				else // Once the requested API layer is found.
				{
					m_activeAPILayers.push_back(requestedLayer.c_str());
					break;
				}
			}
		}

		std::vector<XrExtensionProperties> extensionProperties;
		XrResult extensionsGotten = utils::getInstanceExtensionProperties(&extensionProperties);
		if (extensionsGotten != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not get the extensions from the OpenXR instance.\n";
			return;
		}

		// Determine active instance extensions based on what is requested and what the instance provides.
		for (auto& requestedExtension : m_requestedExtensions)
		{
			bool found = false;
			for (auto& extensionProperty : extensionProperties)
			{
				// If the requested extension and the current gotten extension do not match, check next gotten extension.
				if (strcmp(requestedExtension.c_str(), extensionProperty.extensionName) != 0)
				{
					continue;
				}
				else // Once the requested instance extension is found.
				{
					m_activeExtensions.push_back(requestedExtension.c_str());
					found = true;
					break;
				}
			}
			if (!found)
			{
				std::cerr << "XR_INIT_ERROR could not find requested OpenXR extension: " << requestedExtension << "\n";
			}
		}

		XrResult instanceCreated = utils::createInstance(app, m_activeAPILayers, m_activeExtensions, &m_instance);
		if (instanceCreated != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not create instance.\n";
			return;
		}
		XrResult gotInstanceProperties = utils::getInstanceProperties(m_instance, &m_instanceProperties);
		if (gotInstanceProperties != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR could not get instance properties.\n";
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

		m_views.resize(m_viewConfigs.size(), { XR_TYPE_VIEW });
		m_colorSwapchainInfos.resize(m_viewConfigs.size());
		m_depthSwapchainInfos.resize(m_viewConfigs.size());

		for (uint32_t i = 0; i < m_viewConfigs.size(); ++i)
		{
			XrSwapchainUsageFlags colorFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
			uint64_t colorFormat = GL_SRGB8_ALPHA8;
			XrResult createdColorSwapchain = utils::createSwapchain(colorFlags, colorFormat, m_viewConfigs[i], m_session, &m_colorSwapchainInfos[i].swapchain);
			m_colorSwapchainInfos[i].format = colorFormat;
			if (createdColorSwapchain != XR_SUCCESS)
			{
				std::cerr << "XR_INIT_ERROR could not create color swapchain.\n" << createdColorSwapchain;
				return;
			}

			XrSwapchainUsageFlags depthFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			uint64_t depthFormat = GL_DEPTH_COMPONENT32F;
			XrResult createdDepthSwapchain = utils::createSwapchain(depthFlags, depthFormat, m_viewConfigs[i], m_session, &m_depthSwapchainInfos[i].swapchain);
			m_depthSwapchainInfos[i].format = depthFormat;
			if (createdDepthSwapchain != XR_SUCCESS)
			{
				std::cerr << "XR_INIT_ERROR could not create depth swapchain.\n" << createdDepthSwapchain;
				return;
			}

			XrResult gotColorImages = utils::enumerateSwapchainImages(m_colorSwapchainInfos[i].swapchain, &m_colorSwapchainInfos[i].images);
			if (gotColorImages != XR_SUCCESS)
			{
				std::cerr << "XR_INIT_ERROR could not acquire color swapchain images.\n" << gotColorImages;
				return;
			}
			XrResult gotDepthImages = utils::enumerateSwapchainImages(m_depthSwapchainInfos[i].swapchain, &m_depthSwapchainInfos[i].images);
			if (gotDepthImages != XR_SUCCESS)
			{
				std::cerr << "XR_INIT_ERROR could not acquire color swapchain images.\n" << gotDepthImages;
				return;
			}
		}
		std::cout << "[OPENXR] Established connection!\n";
	}

	/// Set up the input system with a pre-built list of all needed inputs.
	void OpenXRManager::InputHandler::createActionBindings()
	{
		if (m_manager.m_instance == XR_NULL_HANDLE) return; // Do not proceed if no connection was established.

		// Create action sets.
		XrActionSet gameplaySet;
		utils::input::createActionSet(m_manager.m_instance, "gameplay", 0, &gameplaySet);
		m_actionSets.push_back(gameplaySet);
		
		// Create actions with their respective paths.
		XrAction leftGripAction;
		XrResult actionMade = utils::input::createAction(gameplaySet, "left_grip_pose", XR_ACTION_TYPE_POSE_INPUT, &leftGripAction);
		XrPath leftGripPath;
		utils::input::createPath(m_manager.m_instance, "/user/hand/left/input/grip/pose", &leftGripPath);
		m_actionBindings.push_back({ leftGripAction, leftGripPath, XR_ACTION_TYPE_POSE_INPUT, "left_grip_pose" });
		std::cout << "action made: " << actionMade << "\n";
		XrAction rightGripAction;
		utils::input::createAction(gameplaySet, "right_grip_pose", XR_ACTION_TYPE_POSE_INPUT, &rightGripAction);
		XrPath rightGripPath;
		utils::input::createPath(m_manager.m_instance, "/user/hand/right/input/grip/pose", &rightGripPath);
		m_actionBindings.push_back({ rightGripAction, rightGripPath, XR_ACTION_TYPE_POSE_INPUT, "right_grip_pose" });

		XrAction leftAimAction;
		utils::input::createAction(gameplaySet, "left_aim_pose", XR_ACTION_TYPE_POSE_INPUT, &leftAimAction);
		XrPath leftAimPath;
		utils::input::createPath(m_manager.m_instance, "/user/hand/left/input/aim/pose", &leftAimPath);
		m_actionBindings.push_back({ leftAimAction, leftAimPath, XR_ACTION_TYPE_POSE_INPUT, "left_aim_pose" });

		XrAction rightAimAction;
		utils::input::createAction(gameplaySet, "right_aim_pose", XR_ACTION_TYPE_POSE_INPUT, &rightAimAction);
		XrPath rightAimPath;
		utils::input::createPath(m_manager.m_instance, "/user/hand/right/input/aim/pose", &rightAimPath);
		m_actionBindings.push_back({ rightAimAction, rightAimPath, XR_ACTION_TYPE_POSE_INPUT, "right_aim_pose" });

		XrPath leftHandPath, rightHandPath;
		utils::input::createPath(m_manager.m_instance, "/user/hand/left", &leftHandPath);
		utils::input::createPath(m_manager.m_instance, "/user/hand/right", &rightHandPath);

		// Create action spaces for left and right hands.
		utils::input::createActionSpace(m_manager.m_session, m_actionBindings[0].action, leftHandPath, & m_leftGripSpace);
		utils::input::createActionSpace(m_manager.m_session, m_actionBindings[1].action, rightHandPath, &m_rightGripSpace);
		utils::input::createActionSpace(m_manager.m_session, m_actionBindings[2].action, leftHandPath, &m_leftAimSpace);
		utils::input::createActionSpace(m_manager.m_session, m_actionBindings[3].action, rightHandPath, &m_rightAimSpace);

		utils::input::createPath(m_manager.m_instance, "/interaction_profiles/khr/simple_controller", &m_interactionProfilePath);

		std::vector<XrActionSuggestedBinding> suggestedBindings = utils::input::generateSuggestedBindings(m_actionBindings);

		XrResult bindingsSuggested = utils::input::suggestBindings(m_manager.m_instance, m_interactionProfilePath, suggestedBindings);
		if (bindingsSuggested != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR failed to suggest bindings: " << bindingsSuggested <<"\n";
			return;
		}

		XrResult attachedActionSets = utils::input::attachSessionActionSets(m_manager.m_session, m_actionSets);
		if (attachedActionSets != XR_SUCCESS)
		{
			std::cerr << "XR_INIT_ERROR failed to attach action sets to session.\n";
			return;
		}
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
					m_currentSessionState = { XR_SESSION_STATE_IDLE, "IDLE" };
					break;
				}
				case XR_SESSION_STATE_READY:
				{
					m_currentSessionState = { XR_SESSION_STATE_READY, "READY" };
					XrSessionBeginInfo info = { XR_TYPE_SESSION_BEGIN_INFO };
					info.primaryViewConfigurationType = m_viewType; // Two views
					XrResult beganSession = xrBeginSession(m_session, &info);
					if (beganSession != XR_SUCCESS)
					{
						std::cerr << beganSession << ": XR_RUNTIME_ERROR could not begin session.\n";
					}
					else
					{
						sessionRunning = true;
					}
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
					XrResult endedSession = xrEndSession(m_session);
					if (endedSession != XR_SUCCESS)
					{
						std::cerr << endedSession << ": XR_RUNTIME_ERROR could not end session.\n";
					}
					else
					{
						sessionRunning = false;
					}
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

	/// Updates the current state of input actions.
	void OpenXRManager::InputHandler::syncInputs()
	{
		XrResult syncedActions = utils::input::syncActions(m_manager.m_session, m_actionSets);
		if (syncedActions != XR_SUCCESS)
		{
			std::cerr << "XR_RUNTIME_ERROR could not sync inputs: " << syncedActions << "\n";
		}
	}

	void OpenXRManager::waitRenderToEye(uint32_t eyeIndex)
	{
		if (eyeIndex >= m_views.size())
		{
			std::cerr << "XR_RUNTIME_ERROR could not wait to render to eye, eye index out of bounds.\n";
			return;
		}

		if (m_colorSwapchainInfos.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR no swapchain color images on eye " << eyeIndex << "\n";
			return;
		}
		XrResult waitedColor = utils::waitSwapchainImage(m_colorSwapchainInfos[eyeIndex].swapchain);
		if (waitedColor != XR_SUCCESS)
		{
			std::cerr << waitedColor << ": XR_RUNTIME_ERROR could not wait to write to color swapchain on eye " << eyeIndex << ".\n";
		}

		if (m_depthSwapchainInfos.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR no swapchain depth images on eye " << eyeIndex << "\n";
			return;
		}
		XrResult waitedDepth = utils::waitSwapchainImage(m_depthSwapchainInfos[eyeIndex].swapchain);
		if (waitedDepth != XR_SUCCESS)
		{
			std::cerr << waitedDepth << ": XR_RUNTIME_ERROR could not wait to write to depth swapchain on eye " << eyeIndex << ".\n";
		}
	}

	void OpenXRManager::endRenderToEye(uint32_t eyeIndex)
	{
		if (m_colorSwapchainInfos.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR no color swapchain infos to release images from.\n";
			return;
		}
		else if (eyeIndex >= m_views.size())
		{
			std::cerr << "XR_RUNTIME_ERROR could not end render to eye, eye index out of bounds.\n";
			return;
		}
		XrResult colorImageReleased = utils::releaseSwapchainImage(m_colorSwapchainInfos[eyeIndex].swapchain);
		if (colorImageReleased != XR_SUCCESS)
		{
			std::cerr << colorImageReleased << ": XR_RUNTIME_ERROR could not release swapchain color image on eye " << eyeIndex << ".\n";
			return;
		}

		if (m_depthSwapchainInfos.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR no depth swapchain infos to release images from.\n";
			return;
		}
		else if (eyeIndex >= m_views.size())
		{
			std::cerr << "XR_RUNTIME_ERROR could not end render to eye, eye index out of bounds.\n";
			return;
		}
		XrResult depthImageReleased = utils::releaseSwapchainImage(m_depthSwapchainInfos[eyeIndex].swapchain);
		if (depthImageReleased != XR_SUCCESS)
		{
			std::cerr << colorImageReleased << ": XR_RUNTIME_ERROR could not release swapchain depth image on eye " << eyeIndex << ".\n";
			return;
		}
	}

	void OpenXRManager::waitFrame()
	{
		if (m_session == XR_NULL_HANDLE)
		{
			std::cerr << "XR_RUNTIME_ERROR could not wait frame, session does not exist.\n";
			return;
		}
		XrFrameWaitInfo info = { XR_TYPE_FRAME_WAIT_INFO };
		XrResult waitedFrame = xrWaitFrame(m_session, &info, &m_frameState);
		if (waitedFrame != XR_SUCCESS)
		{
			std::cerr << "XR_RUNTIME_ERROR could not wait frame.\n";
		}
	}

	void OpenXRManager::beginFrame()
	{
		if (m_session == XR_NULL_HANDLE)
		{
			std::cerr << "XR_RUNTIME_ERROR could not begin frame, session does not exist.\n";
			return;
		}
		XrFrameBeginInfo info = { XR_TYPE_FRAME_BEGIN_INFO };
		XrResult beganFrame = xrBeginFrame(m_session, &info);
		if (beganFrame != XR_SUCCESS)
		{
			std::cerr << "XR_RUNTIME_ERROR could not begin frame.\n";
		}

		XrResult gotViews = utils::getViews(m_viewType, m_frameState, m_referenceSpace, m_session, &m_views);
		if (gotViews != XR_SUCCESS)
		{
			std::cerr << gotViews << ": XR_RUNTIME_ERROR could not locate views." << "\n";
		}
	}

	void OpenXRManager::endFrame()
	{
		if (m_session == XR_NULL_HANDLE)
		{
			std::cerr << "XR_RUNTIME_ERROR could not end frame, session does not exist.\n";
			return;
		}
		const uint32_t eyeCount = getEyeCount();

		std::vector<XrCompositionLayerBaseHeader*> layers;
		std::vector<XrCompositionLayerProjectionView> projectionViews(eyeCount);
		XrCompositionLayerProjection layerProjection = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };

		for (uint32_t i = 0; i < eyeCount; ++i)
		{
			projectionViews[i] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };

			projectionViews[i].pose = m_views[i].pose;
			projectionViews[i].fov = m_views[i].fov;
			projectionViews[i].subImage.swapchain = m_colorSwapchainInfos[i].swapchain;
			projectionViews[i].subImage.imageRect.offset = { 0, 0 };
			projectionViews[i].subImage.imageRect.extent = 
			{
				(int32_t)m_viewConfigs[i].recommendedImageRectWidth,
				(int32_t)m_viewConfigs[i].recommendedImageRectHeight
			};
			projectionViews[i].subImage.imageArrayIndex = 0;
		}
		layerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
		layerProjection.space = m_referenceSpace;
		layerProjection.viewCount = eyeCount;
		layerProjection.views = projectionViews.data();
		layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layerProjection));

		XrFrameEndInfo info = { XR_TYPE_FRAME_END_INFO };
		info.displayTime = m_frameState.predictedDisplayTime;
		info.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
		info.layerCount = (uint32_t)layers.size();
		info.layers = layers.data();

		XrResult endedFrame = xrEndFrame(m_session, &info);

		if (endedFrame != XR_SUCCESS)
		{
			std::cerr << "XR_RUNTIME_ERROR could not end frame.\n";
		}
	}

	uint32_t OpenXRManager::getEyeCount()
	{
		if (m_connectedSystemID != XR_NULL_SYSTEM_ID && m_views.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR no eyes were found on the connected device.\n";
			return 0;
		}
		return m_views.size();
	}
	XrViewConfigurationView OpenXRManager::getViewConfig(uint32_t eyeIndex)
	{
		if (m_viewConfigs.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get view configurations, no view configs initialized.\n";
			return XrViewConfigurationView();
		}
		else if (eyeIndex >= m_viewConfigs.size())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get view config, eye index out of bounds.\n";
			return XrViewConfigurationView();
		}
		return m_viewConfigs[eyeIndex];
	}
	XrView OpenXRManager::getView(uint32_t eyeIndex)
	{
		if (m_views.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get view, no views initialized.\n";
			return XrView();
		}
		else if (eyeIndex >= m_views.size())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get view, eye index out of bounds.\n";
			return XrView();
		}
		return m_views[eyeIndex];
	}

	uint32_t OpenXRManager::getFrameColorImage(uint32_t eyeIndex)
	{
		if (m_colorSwapchainInfos.empty() || m_colorSwapchainInfos[eyeIndex].images.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get frame color image, initialized incorrectly.\n";
			return 0;
		}
		else if (eyeIndex >= m_views.size())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get frame depth image, eye index out of bounds.\n";
			return 0;
		}

		uint32_t imageIndex;
		XrResult imageIndexAcquired = utils::acquireSwapchainImage(m_colorSwapchainInfos[eyeIndex].swapchain, &imageIndex);
		if (imageIndexAcquired != XR_SUCCESS)
		{
			std::cerr << imageIndexAcquired << ": XR_RUNTIME_ERROR could not acquire swapchain color image index on eye " << eyeIndex << "\n";
			return 0;
		}

		return m_colorSwapchainInfos[eyeIndex].images[imageIndex].image; // Get image by current eye and next available image.
	}
	uint32_t OpenXRManager::getFrameDepthImage(uint32_t eyeIndex)
	{
		if (m_depthSwapchainInfos.empty() || m_depthSwapchainInfos[eyeIndex].images.empty())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get frame depth image, initialized incorrectly.\n";
			return 0;
		}
		else if (eyeIndex >= m_views.size())
		{
			std::cerr << "XR_RUNTIME_ERROR could not get frame depth image, eye index out of bounds.\n";
			return 0;
		}

		uint32_t imageIndex;
		XrResult imageIndexAcquired = utils::acquireSwapchainImage(m_depthSwapchainInfos[eyeIndex].swapchain, &imageIndex);
		if (imageIndexAcquired != XR_SUCCESS)
		{
			std::cerr << imageIndexAcquired << ": XR_RUNTIME_ERROR could not acquire swapchain depth image index on eye " << eyeIndex << "\n";
			return 0;
		}

		return m_depthSwapchainInfos[eyeIndex].images[imageIndex].image; // Get image by current eye and next available image.
	}

	bool OpenXRManager::InputHandler::isPressed(InputPath path)
	{
		if (path >= m_actionBindings.size())
		{
			std::cerr << "XR_RUNTIME_ERROR invalid input path index.\n";
			return false;
		}
		if (m_actionBindings[path].type == XR_ACTION_TYPE_BOOLEAN_INPUT)
		{
			XrActionStateBoolean boolState = { XR_TYPE_ACTION_STATE_BOOLEAN };
			XrResult gotActionState = utils::input::getActionStateBoolean(m_manager.m_session, m_actionBindings[path].action, &boolState);
			if (gotActionState != XR_SUCCESS)
			{
				std::cerr << "XR_RUNTIME_ERROR could not get boolean value from input path.\n";
				return false;
			}
			return boolState.currentState;
		}
		else if (m_actionBindings[path].type == XR_ACTION_TYPE_FLOAT_INPUT)
		{
			return getFloatValue(path) > 0.0f ? true : false;
		}
		else
		{
			std::cerr << "XR_RUNTIME_ERROR type mismatch: could not get boolean value from input path.\n";
			return false;
		}
	}
	float OpenXRManager::InputHandler::getFloatValue(InputPath path)
	{
		if (path >= m_actionBindings.size())
		{
			std::cerr << "XR_RUNTIME_ERROR invalid input path index.\n";
			return 0.0f;
		}
		if (m_actionBindings[path].type == XR_ACTION_TYPE_FLOAT_INPUT)
		{
			XrActionStateFloat floatState = { XR_TYPE_ACTION_STATE_FLOAT };
			XrResult gotActionState = utils::input::getActionStateFloat(m_manager.m_session, m_actionBindings[path].action, &floatState);
			if (gotActionState != XR_SUCCESS)
			{
				std::cerr << "XR_RUNTIME_ERROR could not get float value from input path.\n";
				return 0;
			}
			return floatState.currentState;
		}
		else if (m_actionBindings[path].type == XR_ACTION_TYPE_BOOLEAN_INPUT)
		{
			return isPressed(path) ? 1.0f : 0.0f;
		}
		else
		{
			std::cerr << "XR_RUNTIME_ERROR type mismatch: could not get float value from input path.\n";
			return 0.0f;
		}
	}
	ImmersiveEngine::Math::Vector2 OpenXRManager::InputHandler::getVector2Value(InputPath path)
	{
		if (path >= m_actionBindings.size())
		{
			std::cerr << "XR_RUNTIME_ERROR invalid input path index.\n";
			return ImmersiveEngine::Math::Vector2(0, 0);
		}
		if (m_actionBindings[path].type == XR_ACTION_TYPE_VECTOR2F_INPUT)
		{
			XrActionStateVector2f vector2State = { XR_TYPE_ACTION_STATE_VECTOR2F };
			XrResult gotActionState = utils::input::getActionStateVector2f(m_manager.m_session, m_actionBindings[path].action, &vector2State);
			if (gotActionState != XR_SUCCESS)
			{
				std::cerr << "XR_RUNTIME_ERROR could not get vector2 value from input path.\n";
				return ImmersiveEngine::Math::Vector2(0, 0);
			}
			return ImmersiveEngine::Math::Vector2(vector2State.currentState.x, vector2State.currentState.y);
		}
		else
		{
			std::cerr << "XR_RUNTIME_ERROR type mismatch: could not get vector2 value from input path.\n";
			return ImmersiveEngine::Math::Vector2(0, 0);
		}
	}
	utils::input::Pose OpenXRManager::InputHandler::getPoseValue(InputPath path)
	{
		if (path >= m_actionBindings.size())
		{
			std::cerr << "XR_RUNTIME_ERROR invalid input path index.\n";
			return utils::input::Pose{};
		}
		if (m_actionBindings[path].type == XR_ACTION_TYPE_POSE_INPUT)
		{
			XrSpace space;
			if (path == InputPath::leftGripPose)
			{
				space = m_leftGripSpace;
			}
			else if (path == InputPath::rightGripPose)
			{
				space = m_rightGripSpace;
			}
			else if (path == InputPath::leftAimPose)
			{
				space = m_leftAimSpace;
			}
			else if (path == InputPath::rightAimPose)
			{
				space = m_rightAimSpace;
			}
			else
			{
				std::cerr << "XR_RUNTIME_ERROR type mismatch: could not get pose value from input path.\n";
				return utils::input::Pose{ };
			}

			XrSpaceLocation location = { XR_TYPE_SPACE_LOCATION };
			XrResult locatedSpace = utils::locateSpace(space, m_manager.m_referenceSpace, m_manager.m_frameState.predictedDisplayTime, &location);
			if (locatedSpace != XR_SUCCESS)
			{
				std::cerr << "XR_RUNTIME_ERROR could not get pose value from input path.\n";
				return utils::input::Pose{ };
			}
			ImmersiveEngine::Math::Vector3 position(location.pose.position.x, location.pose.position.y, location.pose.position.z);
			ImmersiveEngine::Math::Quaternion orientation(location.pose.orientation.w, location.pose.orientation.x, location.pose.orientation.y, location.pose.orientation.z);
			return utils::input::Pose{ position, orientation };
		}
		else
		{
			std::cerr << "XR_RUNTIME_ERROR type mismatch: could not get pose value from input path.\n";
			return utils::input::Pose{ };
		}
	}

	std::string OpenXRManager::toString()
	{
		std::string layerLst = "";
		for (const std::string& layer : m_activeAPILayers)
		{
			layerLst.append("\n\t");
			layerLst.append(layer);
		}

		std::string extensionsLst = "";
		for (const std::string& extension : m_activeExtensions)
		{
			extensionsLst.append("\n\t");
			extensionsLst.append(extension);
		}

		std::ostringstream oss;
		oss << "\n[OPENXR MANAGER]"
			<< "\nDevice: " << m_connectedSystemProperties.systemName
			<< "\nSession Running: " << (sessionRunning ? "true" : "false")
			<< "\nCurrent State: " << m_currentSessionState.second
			<< "\nRuntime: " << m_instanceProperties.runtimeName << " version " << m_instanceProperties.runtimeVersion
			<< "\nActive API Layers: " << layerLst
			<< "\nActive Extensions: " << extensionsLst
			<< "\n";
		return oss.str();
	}
}