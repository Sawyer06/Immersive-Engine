#ifndef OPEN_XR_MANAGER_CLASS
#define OPEN_XR_MANAGER_CLASS

#include<string>

#include<openxr/openxr.h>

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

			XrSystemId getXRSystemID(XrInstance& instance);
			XrSystemProperties getXRSystemProperties(XrInstance& instance, XrSystemId& systemID);

			XrInstance createInstance();
			XrSession createSession(XrInstance& instance, XrSystemId& systemID);

			void destroyInstance(XrInstance& instance);
			void destroySession(XrSession& session);
		public:
			OpenXRManager();
			~OpenXRManager();

			bool sessionRunning = false;
			
			void establishConnection();
			void pollEvents();

			void waitFrame();
			void beginFrame();
			void endFrame();

			std::string getSessionStateAsString();
			std::string toString();
	};
}
#endif

