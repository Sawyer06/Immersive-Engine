#ifndef OPEN_XR_MANAGER_CLASS
#define OPEN_XR_MANAGER_CLASS

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

namespace ImmersiveEngine::XR
{
	class OpenXRManager
	{
		private:
			XrSystemId m_connectedSystemID = XR_NULL_SYSTEM_ID;
			XrSystemProperties m_connectedSystemProperties = { XR_TYPE_SYSTEM_PROPERTIES };

			XrInstance m_instance = XR_NULL_HANDLE;
			XrSession m_session = XR_NULL_HANDLE;
			std::vector<XrViewConfigurationView> m_views = { };
			std::vector<XrSwapchain> m_swapchains = { };
			std::vector<std::vector<XrSwapchainImageOpenGLKHR>> m_images = { };

			std::pair<XrSessionState, std::string> m_currentSessionState = { XR_SESSION_STATE_UNKNOWN, "XR_SESSION_STATE_UNKNOWN" }; // state, string debug info

			XrViewConfigurationType m_viewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; // Two views
			XrSpace m_referenceSpace = XR_REFERENCE_SPACE_STAGE;

			XrApplicationInfo generateApplicationInfo(std::string appName, uint32_t appVersion, std::string engineName, uint32_t engineVersion);

			XrResult getXRSystemID(XrInstance& instance, XrSystemId* o_systemID);
			XrResult getXRSystemProperties(XrInstance& instance, XrSystemId& systemID, XrSystemProperties* o_systemProperties);
			XrResult getViewConfigurationViews(XrViewConfigurationType& viewType, XrInstance& instance, XrSystemId& systemID, std::vector<XrViewConfigurationView>* o_views);

			XrResult createInstance(XrApplicationInfo& app, XrInstance* o_instance);
			XrResult createSession(XrInstance& instance, XrSystemId& systemID, XrSession* o_session);
			XrResult createSwapchain(XrViewConfigurationView& view, XrSession& session, XrSwapchain* o_swapchain);

			XrResult enumerateSwapchainImages(XrSwapchain& swapchain, std::vector<XrSwapchainImageOpenGLKHR>* o_images);
			XrResult acquireSwapchainImage(XrSwapchain& swapchain, uint32_t* o_index);
			XrResult waitSwapchainImage(XrSwapchain& swapchain);
			XrResult releaseSwapchainImage(XrSwapchain& swapchain);

			XrResult destroyInstance(XrInstance& instance);
			XrResult destroySession(XrSession& session);
			XrResult destroySwapchain(XrSwapchain& swapchain);
		public:
			OpenXRManager();
			~OpenXRManager();

			bool sessionRunning = false;

			void establishConnection();
			void pollEvents();
			
			void startRenderToEye(uint32_t index);
			void endRenderToEye(uint32_t index);

			void waitFrame();
			void beginFrame();
			void endFrame();

			uint32_t getEyeCount();
			std::string toString();
	};
}
#endif

