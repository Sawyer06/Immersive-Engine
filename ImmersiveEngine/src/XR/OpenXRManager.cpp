#include"OpenXRManager.h"

namespace ImmersiveEngine::XR
{
	OpenXRManager::OpenXRManager() {}

	OpenXRManager::~OpenXRManager()
	{
		destroySession(m_session);
		destroyInstance(m_instance);
	}

	/// Set up connection with headset.
	void OpenXRManager::establishConnection()
	{
		
	}

	void OpenXRManager::pollEvents()
	{

	}

	void OpenXRManager::waitFrame()
	{

	}

	void OpenXRManager::beginFrame()
	{

	}

	void OpenXRManager::endFrame()
	{

	}

	XrSystemId OpenXRManager::getXRSystemID(XrInstance& instance)
	{
		XrSystemGetInfo info { XR_TYPE_SYSTEM_GET_INFO };
		info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		xrGetSystem(m_instance, &info, &m_connectedSystemID);
	}
	XrSystemProperties OpenXRManager::getXRSystemProperties(XrInstance& instance, XrSystemId& systemID)
	{

	}

	XrInstance OpenXRManager::createInstance()
	{

	}

	XrSession OpenXRManager::createSession(XrInstance& instance, XrSystemId& systemId)
	{

	}

	void OpenXRManager::destroyInstance(XrInstance& instance)
	{
		if (instance != XR_NULL_HANDLE) xrDestroyInstance(instance);
	}
	void OpenXRManager::destroySession(XrSession& session)
	{
		if (session != XR_NULL_HANDLE) xrDestroySession(session);
	}
}