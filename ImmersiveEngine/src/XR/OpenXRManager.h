#ifndef OPEN_XR_MANAGER_CLASS
#define OPEN_XR_MANAGER_CLASS

#include<string>
#include<sstream>
#include<iostream>

#define XR_USE_GRAPHICS_API_OPENGL
#if defined(_WIN32)
#undef WIN32_LEAN_AND_MEAN
#define XR_USE_PLATFORM_WIN32
#include <windows.h>
#endif

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

			std::pair<XrSessionState, std::string> m_currentSessionState = { XR_SESSION_STATE_UNKNOWN, "XR_SESSION_STATE_UNKNOWN" }; // state, string debug info

			XrResult getXRSystemID(XrInstance& instance, XrSystemId* o_systemID);
			XrResult getXRSystemProperties(XrInstance& instance, XrSystemId& systemID, XrSystemProperties* o_systemProperties);

			XrApplicationInfo createApplicationInfo(std::string appName, uint32_t appVersion, std::string engineName, uint32_t engineVersion);

			XrResult createInstance(XrApplicationInfo& app, XrInstance* o_instance);
			XrResult createSession(XrInstance& instance, XrSystemId& systemID, XrSession* o_session);

			XrResult destroyInstance(XrInstance& instance);
			XrResult destroySession(XrSession& session);
		public:
			OpenXRManager();
			~OpenXRManager();

			bool sessionRunning = false;

			void establishConnection();
			void pollEvents();

			void waitFrame();
			void beginFrame();
			void endFrame();

			std::string toString();
	};
}
#endif

