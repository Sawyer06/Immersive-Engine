#ifndef OPEN_XR_MANAGER_CLASS_H
#define OPEN_XR_MANAGER_CLASS_H

#include"OpenXRManagerUtils.h"

namespace ImmersiveEngine::XR
{
	struct ViewConfig
	{
		uint32_t width = 0;
		uint32_t height = 0;

		ViewConfig() = default;
		ViewConfig(uint32_t w, uint32_t h) :
			width(w), height(h) { }
	};
	struct HMDSpace
	{
		ImmersiveEngine::Math::Vector3 position = { 0, 0, 0 };
		ImmersiveEngine::Math::Quaternion orientation = { 1, 0, 0, 0 };

		HMDSpace() = default;
		HMDSpace(ImmersiveEngine::Math::Vector3 pos, ImmersiveEngine::Math::Quaternion rot) :
			position(pos), orientation(rot) { }
	};

	class OpenXRManager
	{
		private:
			XrSystemId m_connectedSystemID = XR_NULL_SYSTEM_ID;
			XrSystemProperties m_connectedSystemProperties = { XR_TYPE_SYSTEM_PROPERTIES };

			XrInstance m_instance = XR_NULL_HANDLE;
			XrSession m_session = XR_NULL_HANDLE;
			std::vector<XrView> m_views = { };
			std::vector<XrViewConfigurationView> m_viewConfigs = { };
			std::vector<XrSwapchain> m_swapchains = { };
			std::vector<std::vector<XrSwapchainImageOpenGLKHR>> m_images = { };
			XrSpace m_referenceSpace = XR_NULL_HANDLE;
			XrFrameState m_frameState = { XR_TYPE_FRAME_STATE };

			std::pair<XrSessionState, std::string> m_currentSessionState = { XR_SESSION_STATE_UNKNOWN, "XR_SESSION_STATE_UNKNOWN" }; // state, string debug info

			std::vector<const char*> m_enabledExtensions = { XR_KHR_OPENGL_ENABLE_EXTENSION_NAME };

			XrViewConfigurationType m_viewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; // Two views
			XrReferenceSpaceType m_referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
		public:
			OpenXRManager();
			~OpenXRManager();

			bool sessionRunning = false;

			void establishConnection();
			void pollEvents();
			
			void waitRenderToEye(uint32_t eyeIndex);
			void endRenderToEye(uint32_t eyeIndex);

			void waitFrame();
			void beginFrame();
			void endFrame();

			uint32_t getEyeCount();
			ViewConfig getViewConfig(uint32_t eyeIndex);
			HMDSpace getHMDSpace();
			uint32_t getFrameImage(uint32_t eyeIndex);
			

			std::string toString();
	};
}
#endif

