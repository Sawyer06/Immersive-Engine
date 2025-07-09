#ifndef OPEN_XR_MANAGER_CLASS
#define OPEN_XR_MANAGER_CLASS

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
			bool m_applicationRunning = true;
			bool m_sessionRunning = false;
	};
}
#endif

