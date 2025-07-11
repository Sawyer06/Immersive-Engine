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

			bool m_applicationRunning = true;
			bool m_sessionRunning = false;

			void createInstance();
			void destroyInstance();
			void createSession();
			void destroySession();

			void pollEvents();

			void createDebugMessenger();
			void destroyDebugMessenger();
			void getInstanceProperties();
			void getSystemID();
	};
}
#endif

