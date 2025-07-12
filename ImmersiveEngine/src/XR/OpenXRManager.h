#ifndef OPEN_XR_MANAGER_CLASS
#define OPEN_XR_MANAGER_CLASS

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_OPENGL

#include<openxr/DebugOutput.h>
#include<openxr/GraphicsAPI_OpenGL.h>
#include<openxr/OpenXRDebugUtils.h>

namespace ImmersiveEngine::XR
{
	class OpenXRManager
	{
		public:
			OpenXRManager(GraphicsAPI_Type apiType);
			~OpenXRManager() = default;

			void run();
		private:
			XrInstance m_xrInstance = {};

			std::vector<const char*> m_activeAPILayers = {};
			std::vector<const char*> m_activeInstanceExtensions = {};
			std::vector<std::string> m_apiLayers = {};
			std::vector<std::string> m_instanceExtensions = {};

			XrDebugUtilsMessengerEXT m_debugUtilsMessenger = {};

			XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
			XrSystemId m_systemID = {};
			XrSystemProperties m_systemProperties = { XR_TYPE_SYSTEM_PROPERTIES };

			GraphicsAPI_Type m_apiType = UNKNOWN;
			std::unique_ptr<GraphicsAPI> m_graphicsAPI = nullptr;

			XrSession m_session = XR_NULL_HANDLE;
			XrSessionState m_sessionState = XR_SESSION_STATE_UNKNOWN;

			std::vector<XrViewConfigurationType> m_applicationViewConfigurations = { XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO };
			std::vector<XrViewConfigurationType> m_viewConfigurations;
			XrViewConfigurationType m_viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
			std::vector<XrViewConfigurationView> m_viewConfigurationViews;

			struct SwapchainInfo 
			{
				XrSwapchain swapchain = XR_NULL_HANDLE;
				int64_t swapchainFormat = 0;
				std::vector<void*> imageViews;
			};
			std::vector<SwapchainInfo> m_colorSwapchainInfos = {};
			std::vector<SwapchainInfo> m_depthSwapchainInfos = {};

			std::vector<XrEnvironmentBlendMode> m_applicationEnvironmentBlendModes = { XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE };
			std::vector<XrEnvironmentBlendMode> m_environmentBlendModes = {};
			XrEnvironmentBlendMode m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;

			XrSpace m_localSpace = XR_NULL_HANDLE;
			struct RenderLayerInfo 
			{
				XrTime predictedDisplayTime;
				std::vector<XrCompositionLayerBaseHeader*> layers;
				XrCompositionLayerProjection layerProjection = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
				std::vector<XrCompositionLayerProjectionView> layerProjectionViews;
			};

			bool m_applicationRunning = true;
			bool m_sessionRunning = false;

			void createInstance();
			void destroyInstance();
			
			void createSession();
			void destroySession();

			void createSwapchains();
			void destroySwapchains();

			void createReferenceSpace();
			void destroyReferenceSpace();

			void renderFrame();
			bool renderLayer(RenderLayerInfo& renderLayerInfo);
			void pollEvents();
			
			void getInstanceProperties();
			void getSystemID();
			void getViewConfigurationViews();
			void getEnvironmentBlendModes();

			void createDebugMessenger();
			void destroyDebugMessenger();
	};
}
#endif

