#ifndef OPEN_XR_MANAGER_UTILS_H
#define OPEN_XR_MANAGER_UTILS_H

#include<string>
#include<sstream>
#include<iostream>
#include<vector>

#define XR_USE_GRAPHICS_API_OPENGL
#if defined(_WIN32)
#undef WIN32_LEAN_AND_MEAN
#define XR_USE_PLATFORM_WIN32
#include <windows.h>
#endif

#include<glad/glad.h>
#include<openxr/openxr.h>
#include<openxr/openxr_platform.h>

namespace utils
{
	XrApplicationInfo generateApplicationInfo(std::string appName, uint32_t appVersion, std::string engineName, uint32_t engineVersion);

	XrResult getGraphicsRequirements(XrInstance& instance, XrSystemId& systemID, XrGraphicsRequirementsOpenGLKHR* o_graphicsRequirements);
	XrResult getXRSystemID(XrInstance& instance, XrSystemId* o_systemID);
	XrResult getXRSystemProperties(XrInstance& instance, XrSystemId& systemID, XrSystemProperties* o_systemProperties);
	XrResult getViewConfigurationViews(XrViewConfigurationType& viewType, XrInstance& instance, XrSystemId& systemID, std::vector<XrViewConfigurationView>* o_viewConfigs);
	XrResult getViews(XrViewConfigurationType& viewType, XrFrameState& frameState, XrSpace& space, XrSession& session, uint32_t& viewCount, std::vector<XrView>* o_views);

	XrResult createInstance(XrApplicationInfo& app, std::vector<const char*> extensions, XrInstance* o_instance);
	XrResult createSession(XrInstance& instance, XrSystemId& systemID, XrSession* o_session);
	XrResult createSwapchain(XrViewConfigurationView& view, XrSession& session, XrSwapchain* o_swapchain);
	XrResult createReferenceSpace(XrReferenceSpaceType& type, XrSession& session, XrSpace* o_referenceSpace);

	XrResult enumerateSwapchainImages(XrSwapchain& swapchain, std::vector<XrSwapchainImageOpenGLKHR>* o_images);
	XrResult acquireSwapchainImage(XrSwapchain& swapchain, uint32_t* o_index);
	XrResult waitSwapchainImage(XrSwapchain& swapchain);
	XrResult releaseSwapchainImage(XrSwapchain& swapchain);

	XrResult destroyInstance(XrInstance& instance);
	XrResult destroySession(XrSession& session);
	XrResult destroySwapchain(XrSwapchain& swapchain);
}
#endif