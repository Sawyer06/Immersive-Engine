#ifndef OPEN_XR_MANAGER_CLASS_H
#define OPEN_XR_MANAGER_CLASS_H

#include"OpenXRUtils.h"

namespace ImmersiveEngine::XR
{
	class OpenXRManager
	{
		private:
			XrSystemId m_connectedSystemID = XR_NULL_SYSTEM_ID;
			XrSystemProperties m_connectedSystemProperties = { XR_TYPE_SYSTEM_PROPERTIES };

			XrInstance m_instance = XR_NULL_HANDLE;
			XrInstanceProperties m_instanceProperties = { XR_TYPE_INSTANCE_PROPERTIES };

			XrSession m_session = XR_NULL_HANDLE;
			std::vector<XrView> m_views = { };
			std::vector<XrViewConfigurationView> m_viewConfigs = { };
			
			std::vector<utils::SwapchainInfo> m_colorSwapchainInfos = { };
			std::vector<utils::SwapchainInfo> m_depthSwapchainInfos = { };

			XrSpace m_referenceSpace = XR_NULL_HANDLE;
			XrFrameState m_frameState = { XR_TYPE_FRAME_STATE };

			std::pair<XrSessionState, std::string> m_currentSessionState = { XR_SESSION_STATE_UNKNOWN, "XR_SESSION_STATE_UNKNOWN" }; // state, string debug info

			std::vector<const char*> m_activeAPILayers = { };
			std::vector<const char*> m_activeExtensions = { };

			std::vector<std::string> m_requestedAPILayers = { };
			std::vector<std::string> m_requestedExtensions = { };

			XrViewConfigurationType m_viewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; // Two views
			XrReferenceSpaceType m_referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
		public:
			OpenXRManager();
			~OpenXRManager();

			class InputHandler
			{
				private:
					OpenXRManager& m_manager;

					std::vector<XrActionSet> m_actionSets;
					std::vector<utils::input::ActionBinding> m_actionBindings;

					XrPath m_interactionProfilePath = XR_NULL_PATH;

					XrSpace m_leftGripSpace = XR_NULL_HANDLE;
					XrSpace m_rightGripSpace = XR_NULL_HANDLE;
					XrSpace m_leftAimSpace = XR_NULL_HANDLE;
					XrSpace m_rightAimSpace = XR_NULL_HANDLE;
				public:
					InputHandler(OpenXRManager& manager);
					~InputHandler();

					enum InputPath
					{
						leftGripPose,
						rightGripPose,
						leftAimPose,
						rightAimPose,
					};

					void createActionBindings();
					void syncInputs();
					
					bool isPressed(InputPath path);
					float getFloatValue(InputPath path);
					ImmersiveEngine::Math::Vector2 getVector2Value(InputPath path);
					utils::input::Pose getPoseValue(InputPath path);
			};
			friend class InputHandler;

			InputHandler input;

			bool sessionRunning = false;

			void establishConnection();
			void pollEvents();
			
			void waitRenderToEye(uint32_t eyeIndex);
			void endRenderToEye(uint32_t eyeIndex);

			void waitFrame();
			void beginFrame();
			void endFrame();

			uint32_t getEyeCount();
			XrViewConfigurationView getViewConfig(uint32_t eyeIndex);
			XrView getView(uint32_t eyeIndex);

			uint32_t getFrameColorImage(uint32_t eyeIndex);
			uint32_t getFrameDepthImage(uint32_t eyeIndex);

			std::string toString();
	};
}

using InputPath = ImmersiveEngine::XR::OpenXRManager::InputHandler::InputPath;
#endif

