#include"OpenXRUtils.h"

namespace ImmersiveEngine::XR::utils
{
	XrApplicationInfo generateApplicationInfo(std::string appName, uint32_t appVersion, std::string engineName, uint32_t engineVersion)
	{
		XrApplicationInfo app;
		strncpy(app.applicationName, appName.c_str(), XR_MAX_APPLICATION_NAME_SIZE);
		app.applicationVersion = appVersion;
		strncpy(app.engineName, engineName.c_str(), XR_MAX_ENGINE_NAME_SIZE);
		app.engineVersion = engineVersion;
		app.apiVersion = XR_CURRENT_API_VERSION;

		return app;
	}

	XrResult getInstanceProperties(XrInstance& instance, XrInstanceProperties* properties)
	{
		return xrGetInstanceProperties(instance, properties);
	}
	XrResult getGraphicsRequirements(XrInstance& instance, XrSystemId& systemID, XrGraphicsRequirementsOpenGLKHR* o_graphicsRequirements)
	{
		PFN_xrGetOpenGLGraphicsRequirementsKHR pfnGetOpenGLGraphicsRequirementsKHR = nullptr;
		xrGetInstanceProcAddr(instance, "xrGetOpenGLGraphicsRequirementsKHR", (PFN_xrVoidFunction*)(&pfnGetOpenGLGraphicsRequirementsKHR));

		if (pfnGetOpenGLGraphicsRequirementsKHR)
		{
			return pfnGetOpenGLGraphicsRequirementsKHR(instance, systemID, o_graphicsRequirements);
		}

		return XR_ERROR_FUNCTION_UNSUPPORTED;
	}
	XrResult getXRSystemID(XrInstance& instance, XrSystemId* o_systemID)
	{
		XrSystemGetInfo info = { XR_TYPE_SYSTEM_GET_INFO };
		info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		return xrGetSystem(instance, &info, o_systemID);
	}
	XrResult getXRSystemProperties(XrInstance& instance, XrSystemId& systemID, XrSystemProperties* o_systemProperties)
	{
		return xrGetSystemProperties(instance, systemID, o_systemProperties);
	}
	XrResult getViewConfigurationViews(XrViewConfigurationType& viewType, XrInstance& instance, XrSystemId& systemID, std::vector<XrViewConfigurationView>* o_viewConfigs)
	{
		uint32_t viewCount = 0;
		xrEnumerateViewConfigurationViews(instance, systemID, viewType, 0, &viewCount, nullptr); // First get number of view configs.
		o_viewConfigs->resize(viewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
		for (auto& viewConfig : *o_viewConfigs) 
		{
			//viewConfig.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
		}

		return xrEnumerateViewConfigurationViews(instance, systemID, viewType, viewCount, &viewCount, o_viewConfigs->data()); // Populate view configs list.
	}
	XrResult getViews(XrViewConfigurationType& viewType, XrFrameState& frameState, XrSpace& space, XrSession& session, std::vector<XrView>* o_views)
	{
		for (auto& view : *o_views)
		{
			//view.type = XR_TYPE_VIEW;
		}

		XrViewLocateInfo info = { XR_TYPE_VIEW_LOCATE_INFO };
		info.viewConfigurationType = viewType;
		info.displayTime = frameState.predictedDisplayTime;
		info.space = space;

		XrViewState viewState = { XR_TYPE_VIEW_STATE };
		uint32_t viewCount = 0;

		return xrLocateViews(session, &info, &viewState, o_views->size(), &viewCount, o_views->data());
	}
	XrResult getAPILayerProperties(std::vector<XrApiLayerProperties>* o_properties)
	{
		uint32_t apiLayerCount = 0;
		xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr);
		o_properties->resize(apiLayerCount, { XR_TYPE_API_LAYER_PROPERTIES });
		return xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, o_properties->data());
	}
	XrResult getInstanceExtensionProperties(std::vector<XrExtensionProperties>* o_properties)
	{
		uint32_t extensionCount = 0;
		xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr);
		o_properties->resize(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
		return xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, o_properties->data());
	}

	/// Create an instance to communicate with the runtime.
	XrResult createInstance(XrApplicationInfo& app, std::vector<const char*> activeAPILayers, std::vector<const char*> activeExtensions, XrInstance* o_instance)
	{
		XrInstanceCreateInfo info = { XR_TYPE_INSTANCE_CREATE_INFO };
		info.createFlags = 0;
		info.applicationInfo = app;
		
		info.enabledApiLayerCount = static_cast<uint32_t>(activeAPILayers.size());
		info.enabledApiLayerNames = activeAPILayers.data();
		info.enabledExtensionCount = static_cast<uint32_t>(activeExtensions.size());
		info.enabledExtensionNames = activeExtensions.data();

		return xrCreateInstance(&info, o_instance);
	}

	/// Create a session to communicate with the connected device.
	XrResult createSession(XrInstance& instance, XrSystemId& systemID, XrSession* o_session)
	{
		HDC hDC = wglGetCurrentDC();
		HGLRC hGLRC = wglGetCurrentContext();
		wglMakeCurrent(hDC, hGLRC);

		XrSessionCreateInfo info = { XR_TYPE_SESSION_CREATE_INFO };
		XrGraphicsBindingOpenGLWin32KHR graphicsBinding = { XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR };
		graphicsBinding.hDC = hDC;
		graphicsBinding.hGLRC = hGLRC;

		info.next = &graphicsBinding;
		info.systemId = systemID;

		return xrCreateSession(instance, &info, o_session);
	}

	/// Create a swapchain (a reel of buffers) for a specific eye view.
	XrResult createSwapchain(XrSwapchainUsageFlags& flags, uint64_t& format, XrViewConfigurationView& view, XrSession& session, XrSwapchain* o_swapchain)
	{
		XrSwapchainCreateInfo info = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
		info.usageFlags = flags;
		info.format = format;
		info.sampleCount = view.recommendedSwapchainSampleCount;
		info.width = view.recommendedImageRectWidth;
		info.height = view.recommendedImageRectHeight;
		info.faceCount = 1;
		info.arraySize = 1;
		info.mipCount = 1;

		return xrCreateSwapchain(session, &info, o_swapchain);
	}

	/// Create a reference space for headset position and orientation in OpenXR.
	XrResult createReferenceSpace(XrReferenceSpaceType& type, XrSession& session, XrSpace* o_referenceSpace)
	{
		XrReferenceSpaceCreateInfo info = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
		info.referenceSpaceType = type;
		info.poseInReferenceSpace = { {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} };

		return xrCreateReferenceSpace(session, &info, o_referenceSpace);
	}

	XrResult enumerateSwapchainFormats(XrSession& session, std::vector<int64_t>* o_formats)
	{
		uint32_t formatCount = 0;
		xrEnumerateSwapchainFormats(session, 0, &formatCount, nullptr);
		o_formats->resize(formatCount);
		return xrEnumerateSwapchainFormats(session, formatCount, &formatCount, o_formats->data());
	}

	/// Get the index of the next available image in the swapchain.
	XrResult acquireSwapchainImage(XrSwapchain& swapchain, uint32_t* o_index)
	{
		XrSwapchainImageAcquireInfo info = { XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
		return xrAcquireSwapchainImage(swapchain, &info, o_index);
	}

	/// Gets the image list for a swapchain.
	XrResult enumerateSwapchainImages(XrSwapchain& swapchain, std::vector<XrSwapchainImageOpenGLKHR>* o_images)
	{
		uint32_t imageCount = 0;
		xrEnumerateSwapchainImages(swapchain, 0, &imageCount, nullptr);
		o_images->resize(imageCount, { XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR });
		for (auto& image : *o_images)
		{
			//image.type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR;
		}
		return xrEnumerateSwapchainImages(swapchain, imageCount, &imageCount, (XrSwapchainImageBaseHeader*)o_images->data());
	}

	/// Wait on oldest acquired swapchain image to avoid writing to it before the compositer has finished reading it.
	XrResult waitSwapchainImage(XrSwapchain& swapchain)
	{
		XrSwapchainImageWaitInfo info = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
		info.timeout = XR_INFINITE_DURATION;
		return xrWaitSwapchainImage(swapchain, &info);
	}

	/// Give the oldest acquired swapchain image back to the swapchain for reuse.
	XrResult releaseSwapchainImage(XrSwapchain& swapchain)
	{
		XrSwapchainImageReleaseInfo info = { XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
		return xrReleaseSwapchainImage(swapchain, &info);
	}
	XrResult locateSpace(XrSpace& space, XrSpace& baseSpace, XrTime& time, XrSpaceLocation* location)
	{
		return xrLocateSpace(space, baseSpace, time, location);
	}

	XrResult destroyInstance(XrInstance& instance)
	{
		if (instance == XR_NULL_HANDLE) return XR_ERROR_RUNTIME_FAILURE;
		XrResult destroyed = xrDestroyInstance(instance);
		instance = XR_NULL_HANDLE;
		return destroyed;
	}
	XrResult destroySession(XrSession& session)
	{
		if (session == XR_NULL_HANDLE) return XR_ERROR_RUNTIME_FAILURE;
		XrResult destroyed = xrDestroySession(session);
		session = XR_NULL_HANDLE;
		return destroyed;
	}
	XrResult destroySwapchain(XrSwapchain& swapchain)
	{
		if (swapchain == XR_NULL_HANDLE) return XR_ERROR_RUNTIME_FAILURE;
		XrResult destroyed = xrDestroySwapchain(swapchain);
		swapchain = XR_NULL_HANDLE;
		return destroyed;
	}
	XrResult destroySpace(XrSpace& space)
	{
		if (space == XR_NULL_HANDLE) return XR_ERROR_RUNTIME_FAILURE;
		XrResult destroyed = xrDestroySpace(space);
		space = XR_NULL_HANDLE;
		return destroyed;
	}

	namespace input
	{
		/// Connect the action to a path.
		std::vector<XrActionSuggestedBinding> generateSuggestedBindings(std::vector<ActionBinding>& actionBindings)
		{
			std::vector<XrActionSuggestedBinding> suggestedBindings;
			for (ActionBinding actionBinding : actionBindings)
			{
				suggestedBindings.push_back({ actionBinding.action, actionBinding.path });
			}
			return suggestedBindings;
		}

		XrResult createActionSet(XrInstance& instance, const char* name, uint32_t priority, XrActionSet* o_actionSet)
		{
			XrActionSetCreateInfo info = { XR_TYPE_ACTION_SET_CREATE_INFO };
			strcpy(info.actionSetName, name);
			strcpy(info.localizedActionSetName, name);
			info.priority = priority;

			return xrCreateActionSet(instance, &info, o_actionSet);
		}
		XrResult createAction(XrActionSet& actionSet, const char* name, XrActionType type, XrAction* o_action)
		{
			XrActionCreateInfo info = { XR_TYPE_ACTION_CREATE_INFO };
			strcpy(info.actionName, name);
			strcpy(info.localizedActionName, name);
			info.actionType = type;

			return xrCreateAction(actionSet, &info, o_action);
		}
		XrResult createPath(XrInstance& instance, const char* pathString, XrPath* o_path)
		{
			return xrStringToPath(instance, pathString, o_path);
		}
		XrResult createActionSpace(XrSession& session, XrAction& action, XrPath& subactionPath, XrSpace* o_space)
		{
			XrActionSpaceCreateInfo info = { XR_TYPE_ACTION_SPACE_CREATE_INFO };
			info.action = action;
			info.subactionPath = subactionPath;
			info.poseInActionSpace = { {1, 0, 0, 0}, {0, 0, 0} };
			
			return xrCreateActionSpace(session, &info, o_space);
		}

		XrResult getActionStateBoolean(XrSession& session, XrAction& action, XrActionStateBoolean* state)
		{
			XrActionStateGetInfo info = { XR_TYPE_ACTION_STATE_GET_INFO };
			info.action = action;
			
			return xrGetActionStateBoolean(session, &info, state);
		}
		XrResult getActionStateFloat(XrSession& session, XrAction& action, XrActionStateFloat* state)
		{
			XrActionStateGetInfo info = { XR_TYPE_ACTION_STATE_GET_INFO };
			info.action = action;
			
			return xrGetActionStateFloat(session, &info, state);
		}
		XrResult getActionStateVector2f(XrSession& session, XrAction& action, XrActionStateVector2f* state)
		{
			XrActionStateGetInfo info = { XR_TYPE_ACTION_STATE_GET_INFO };
			info.action = action;
			
			return xrGetActionStateVector2f(session, &info, state);
		}

		/// Suggest mapping for actions and paths based on device.
		XrResult suggestBindings(XrInstance& instance, XrPath& interactionProfile, std::vector<XrActionSuggestedBinding>& bindings)
		{
			XrInteractionProfileSuggestedBinding suggestedBindings = { XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
			suggestedBindings.interactionProfile = interactionProfile;
			suggestedBindings.countSuggestedBindings = bindings.size();
			suggestedBindings.suggestedBindings = bindings.data();

			return xrSuggestInteractionProfileBindings(instance, &suggestedBindings);
		}
		XrResult attachSessionActionSets(XrSession& session, std::vector<XrActionSet>& actionSets)
		{
			XrSessionActionSetsAttachInfo info = { XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
			info.countActionSets = actionSets.size();
			info.actionSets = actionSets.data();

			return xrAttachSessionActionSets(session, &info);
		}
		XrResult syncActions(XrSession& session, std::vector<XrActionSet>& actionSets)
		{
			std::vector<XrActiveActionSet> activeActionSets;
			for (XrActionSet set : actionSets)
			{
				activeActionSets.push_back({ set, XR_NULL_PATH });
			}

			XrActionsSyncInfo info = { XR_TYPE_ACTIONS_SYNC_INFO };
			info.countActiveActionSets = activeActionSets.size();
			info.activeActionSets = activeActionSets.data();
			
			return xrSyncActions(session, &info);
		}
		XrResult destroyActionSet(XrActionSet& actionSet)
		{
			if (actionSet != XR_NULL_HANDLE) return XR_ERROR_RUNTIME_FAILURE;
			XrResult destroyed = xrDestroyActionSet(actionSet);
			actionSet = XR_NULL_HANDLE;
			
			return destroyed;
		}
		XrResult destroyAction(XrAction& action)
		{
			if (action != XR_NULL_HANDLE) return XR_ERROR_RUNTIME_FAILURE;
			XrResult destroyed = xrDestroyAction(action);
			action = XR_NULL_HANDLE;
			
			return destroyed;
		}
	}
}