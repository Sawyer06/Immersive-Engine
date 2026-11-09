#include"AnimationClip.h"

namespace ImmersiveEngine::Rendering
{
	AnimationClip::AnimationClip(float duration, float ticksPerSecond) :
		duration(duration), ticksPerSecond(ticksPerSecond)
	{
	}

	AnimationClip::AnimationClip(const AnimationClip& clip) :
		duration(clip.duration), ticksPerSecond(clip.ticksPerSecond),
		positionKeys(clip.positionKeys), orientationKeys(clip.orientationKeys), scaleKeys(clip.scaleKeys)
	{
	}

	/// Calculate position interpolated between the current and previous key frames.
	Math::Vector3 ImmersiveEngine::Rendering::AnimationClip::getBonePositionAtTime(int boneID, float elapsedTime)
	{
		if (positionKeys[boneID].size() == 1)
			return positionKeys[boneID][0].value;

		int keyIndexA = getPositionKeyIndexAtTime(boneID, elapsedTime);
		int keyIndexB = keyIndexA + 1;

		glm::vec3 posA = { positionKeys[boneID][keyIndexA].value.x, positionKeys[boneID][keyIndexA].value.y, positionKeys[boneID][keyIndexA].value.z};
		glm::vec3 posB = { positionKeys[boneID][keyIndexB].value.x, positionKeys[boneID][keyIndexB].value.y, positionKeys[boneID][keyIndexB].value.z};

		float interpolant = (elapsedTime - positionKeys[boneID][keyIndexA].timeStamp) /
			(positionKeys[boneID][keyIndexB].timeStamp - positionKeys[boneID][keyIndexA].timeStamp);

		glm::vec3 interpPosition = glm::mix(posA, posB, interpolant);
		return Math::Vector3(interpPosition.x, interpPosition.y, interpPosition.z);
	}

	/// Calculate orientation interpolated between the current and previous key frames.
	Math::Quaternion ImmersiveEngine::Rendering::AnimationClip::getBoneOrientationAtTime(int boneID, float elapsedTime)
	{
		if (orientationKeys[boneID].size() == 1)
			return orientationKeys[boneID][0].value;

		int keyIndexA = getOrientationKeyIndexAtTime(boneID, elapsedTime);
		int keyIndexB = keyIndexA + 1;

		glm::quat rotA = { orientationKeys[boneID][keyIndexA].value.w, orientationKeys[boneID][keyIndexA].value.x,
			orientationKeys[boneID][keyIndexA].value.y, orientationKeys[boneID][keyIndexA].value.z};
		glm::quat rotB = { orientationKeys[boneID][keyIndexB].value.w, orientationKeys[boneID][keyIndexB].value.x,
			orientationKeys[boneID][keyIndexB].value.y, orientationKeys[boneID][keyIndexB].value.z };;
		float interpolant = (elapsedTime - orientationKeys[boneID][keyIndexA].timeStamp) /
			(orientationKeys[boneID][keyIndexB].timeStamp - orientationKeys[boneID][keyIndexA].timeStamp);

		glm::quat interpOrientation = glm::slerp(rotA, rotB, interpolant);
		return Math::Quaternion(interpOrientation.w, interpOrientation.x, interpOrientation.y, interpOrientation.z);
	}

	/// Calculate scale interpolated between the current and previous key frames.
	Math::Vector3 ImmersiveEngine::Rendering::AnimationClip::getBoneScaleAtTime(int boneID, float elapsedTime)
	{
		if (scaleKeys[boneID].size() == 1)
			return scaleKeys[boneID][0].value;

		int keyIndexA = getScaleKeyIndexAtTime(boneID, elapsedTime);
		int keyIndexB = keyIndexA + 1;

		glm::vec3 posA = { scaleKeys[boneID][keyIndexA].value.x, scaleKeys[boneID][keyIndexA].value.y, scaleKeys[boneID][keyIndexA].value.z };
		glm::vec3 posB = { scaleKeys[boneID][keyIndexB].value.x, scaleKeys[boneID][keyIndexB].value.y, scaleKeys[boneID][keyIndexB].value.z };;
		float interpolant = (elapsedTime - scaleKeys[boneID][keyIndexA].timeStamp) /
			(scaleKeys[boneID][keyIndexB].timeStamp - scaleKeys[boneID][keyIndexA].timeStamp);

		glm::vec3 interpPosition = glm::mix(posA, posB, interpolant);
		return Math::Vector3(interpPosition.x, interpPosition.y, interpPosition.z);
	}

	/// Get the current index of the position keys associated with this bone at the given time.
	int AnimationClip::getPositionKeyIndexAtTime(int boneID, float elapsedTime)
	{
		for (int index = 0; index < positionKeys[boneID].size() - 1; index++)
		{
			if (elapsedTime < positionKeys[boneID][index + 1].timeStamp)
			{
				return index;
			}
		}
		return 0;
	}
	/// Get the current index of the orientation keys associated with this bone at the given time.
	int AnimationClip::getOrientationKeyIndexAtTime(int boneID, float elapsedTime)
	{
		for (int index = 0; index < orientationKeys[boneID].size() - 1; index++)
		{
			if (elapsedTime < orientationKeys[boneID][index + 1].timeStamp)
			{
				return index;
			}
		}
		return 0;
	}
	/// Get the current index of the scale keys associated with this bone at the given time.
	int AnimationClip::getScaleKeyIndexAtTime(int boneID, float elapsedTime)
	{
		for (int index = 0; index < scaleKeys[boneID].size() - 1; index++)
		{
			if (elapsedTime < scaleKeys[boneID][index + 1].timeStamp)
			{
				return index;
			}
		}
		return 0;
	}

}