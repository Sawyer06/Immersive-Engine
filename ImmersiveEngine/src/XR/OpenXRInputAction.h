#ifndef OPEN_XR_INPUT_ACTION
#define OPEN_XR_INPUT_ACTION

#include<iostream>

namespace ImmersiveEngine::XR
{
	class OpenXRInputAction
	{
		template <typename T>
		struct InputActionState
		{
			T value;
			bool changedSinceLastSync;
		};

		private:
			std::string m_name;
			//XrAction m_action = XR_NULL_HANDLE;
			//XrPath m_binding = XR_NULL_PATH;
			//XrActionType m_type = XR_ACTION_TYPE_BOOLEAN_INPUT;
		public:
			//OpenXRInputAction(std::string name, XrActionType type, std::string binding);
			
			template <typename T>
			InputActionState<T> getState();

	};
}
#endif
