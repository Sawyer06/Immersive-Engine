#include"OpenXRManagerUtils.h"

namespace utils
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
		XrSystemGetInfo info{ XR_TYPE_SYSTEM_GET_INFO };
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
		o_viewConfigs->resize(viewCount);
		for (auto& viewConfig : *o_viewConfigs) 
		{
			viewConfig.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
		}

		return xrEnumerateViewConfigurationViews(instance, systemID, viewType, viewCount, &viewCount, o_viewConfigs->data()); // Populate view configs list.
	}
	XrResult getViews(XrViewConfigurationType& viewType, XrFrameState& frameState, XrSpace& space, XrSession& session, uint32_t& viewCount, std::vector<XrView>* o_views)
	{
		for (auto& view : *o_views)
		{
			view.type = XR_TYPE_VIEW;
		}

		XrViewLocateInfo info = { XR_TYPE_VIEW_LOCATE_INFO };
		info.viewConfigurationType = viewType;
		info.displayTime = frameState.predictedDisplayTime;
		info.space = space;

		XrViewState viewState = { XR_TYPE_VIEW_STATE };
		uint32_t viewCountOut;

		return xrLocateViews(session, &info, &viewState, viewCount, &viewCountOut, o_views->data());
	}

	/// Create an instance to communicate with the runtime.
	XrResult createInstance(XrApplicationInfo& app, std::vector<const char*> extensions, XrInstance* o_instance)
	{
		XrInstanceCreateInfo info = { XR_TYPE_INSTANCE_CREATE_INFO };
		info.createFlags = 0;
		info.applicationInfo = app;
		info.enabledExtensionCount = extensions.size();
		info.enabledExtensionNames = extensions.data();

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
	XrResult createSwapchain(XrViewConfigurationView& view, XrSession& session, XrSwapchain* o_swapchain)
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

	/// Create a reference space for headset position and orientation in OpenXR.
	XrResult createReferenceSpace(XrReferenceSpaceType& type, XrSession& session, XrSpace* o_referenceSpace)
	{
		XrReferenceSpaceCreateInfo info = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
		info.referenceSpaceType = type;
		info.poseInReferenceSpace = { {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} };

		return xrCreateReferenceSpace(session, &info, o_referenceSpace);
	}

	/// Get the index of the next available image in the swapchain.
	XrResult acquireSwapchainImage(XrSwapchain& swapchain, uint32_t* o_index)
	{
		XrSwapchainImageAcquireInfo info = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
		return xrAcquireSwapchainImage(swapchain, &info, o_index);
	}

	/// Gets the image list for a swapchain.
	XrResult enumerateSwapchainImages(XrSwapchain& swapchain, std::vector<XrSwapchainImageOpenGLKHR>* o_images)
	{
		uint32_t imageCount = 0;
		xrEnumerateSwapchainImages(swapchain, 0, &imageCount, nullptr);
		o_images->resize(imageCount);
		for (auto& image : *o_images)
		{
			image.type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR;
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
}