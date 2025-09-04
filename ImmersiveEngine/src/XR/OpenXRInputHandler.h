#ifndef OPEN_XR_INPUT_HANDLER_CLASS_H
#define OPEN_XR_INPUT_HANDLER_CLASS_H

#include"OpenXRUtils.h"
#include"OpenXRInputAction.h"

namespace ImmersiveEngine::XR
{
	class OpenXRInputHandler
	{
		private:
			XrInstance m_instance = XR_NULL_HANDLE;
			XrSession m_session = XR_NULL_HANDLE;
		public:
			OpenXRInputHandler();
			~OpenXRInputHandler();
	};
}
#endif
