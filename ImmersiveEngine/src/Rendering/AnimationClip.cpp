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
		auto keyIndices = getPositionKeyIndicesAtTime(boneID, elapsedTime);

		if (keyIndices[1] == -1) // Reached end.
			return positionKeys[keyIndices[0]].value;

		glm::vec3 posA = { positionKeys[keyIndices[0]].value.x, positionKeys[keyIndices[0]].value.y, positionKeys[keyIndices[0]].value.z };
		glm::vec3 posB = { positionKeys[keyIndices[1]].value.x, positionKeys[keyIndices[1]].value.y, positionKeys[keyIndices[1]].value.z };;
		float interpolant = (elapsedTime - positionKeys[keyIndices[0]].timeStamp) /
							(positionKeys[keyIndices[1]].timeStamp - positionKeys[keyIndices[0]].timeStamp);

		glm::vec3 interpPosition = glm::mix(posA, posB, interpolant);
		return Math::Vector3(interpPosition.x, interpPosition.y, interpPosition.z);
	}

	/// Calculate orientation interpolated between the current and previous key frames.
	Math::Quaternion ImmersiveEngine::Rendering::AnimationClip::getBoneOrientationAtTime(int boneID, float elapsedTime)
	{
		auto keyIndices = getOrientationKeyIndicesAtTime(boneID, elapsedTime);

		if (keyIndices[1] == -1) // Reached end.
			return orientationKeys[keyIndices[0]].value;

		glm::quat rotA = { orientationKeys[keyIndices[0]].value.w, orientationKeys[keyIndices[0]].value.x, 
			orientationKeys[keyIndices[0]].value.y, orientationKeys[keyIndices[0]].value.z};
		glm::quat rotB = { orientationKeys[keyIndices[0]].value.w, orientationKeys[keyIndices[1]].value.x, 
			orientationKeys[keyIndices[1]].value.y, orientationKeys[keyIndices[1]].value.z };;
		float interpolant = (elapsedTime - positionKeys[keyIndices[0]].timeStamp) /
			(positionKeys[keyIndices[1]].timeStamp - positionKeys[keyIndices[0]].timeStamp);

		glm::quat interpOrientation = glm::slerp(rotA, rotB, interpolant);
		return Math::Quaternion(interpOrientation.w, interpOrientation.x, interpOrientation.y, interpOrientation.z);
	}

	/// Calculate scale interpolated between the current and previous key frames.
	Math::Vector3 ImmersiveEngine::Rendering::AnimationClip::getBoneScaleAtTime(int boneID, float elapsedTime)
	{
		auto keyIndices = getScaleKeyIndicesAtTime(boneID, elapsedTime);

		if (keyIndices[1] == -1) // Reached end.
			return scaleKeys[keyIndices[0]].value;

		glm::vec3 posA = { positionKeys[keyIndices[0]].value.x, positionKeys[keyIndices[0]].value.y, positionKeys[keyIndices[0]].value.z };
		glm::vec3 posB = { positionKeys[keyIndices[1]].value.x, positionKeys[keyIndices[1]].value.y, positionKeys[keyIndices[1]].value.z };;
		float interpolant = (elapsedTime - positionKeys[keyIndices[0]].timeStamp) /
			(positionKeys[keyIndices[1]].timeStamp - positionKeys[keyIndices[0]].timeStamp);

		glm::vec3 interpPosition = glm::mix(posA, posB, interpolant);
		return Math::Vector3(interpPosition.x, interpPosition.y, interpPosition.z);
	}

	/// Get the current and next indices of the position keys associated with this bone at the given time.
	std::array<int, 2> AnimationClip::getPositionKeyIndicesAtTime(int boneID, float elapsedTime)
	{
		std::array<int, 2> indices = { -1, -1 };
		for (int index = 0; index < positionKeys.size(); index++)
		{
			if (positionKeys[index].boneID == boneID && elapsedTime < positionKeys[index].timeStamp)
			{
				if (indices[0] == -1)
				{
					indices[0] = index; // Current key index of bone.
				}
				else
				{
					indices[1] = index; // Next key index of bone.
					return indices;
				}
			}
		}
		return indices;
	}
	/// Get the current and next indices of the orientation keys associated with this bone at the given time.
	std::array<int, 2> AnimationClip::getOrientationKeyIndicesAtTime(int boneID, float elapsedTime)
	{
		std::array<int, 2> indices = { -1, -1 };
		for (int index = 0; index < orientationKeys.size(); index++)
		{
			if (orientationKeys[index].boneID == boneID && elapsedTime < orientationKeys[index].timeStamp)
			{
				if (indices[0] == -1)
				{
					indices[0] = index; // Current key index of bone.
				}
				else
				{
					indices[1] = index; // Next key index of bone.
					return indices;
				}
			}
		}
		return indices;
	}
	/// Get the current and next indices of the scale keys associated with this bone at the given time.
	std::array<int, 2> AnimationClip::getScaleKeyIndicesAtTime(int boneID, float elapsedTime)
	{
		std::array<int, 2> indices = { -1, -1 };
		for (int index = 0; index < scaleKeys.size(); index++)
		{
			if (scaleKeys[index].boneID == boneID && elapsedTime < scaleKeys[index].timeStamp)
			{
				if (indices[0] == -1)
				{
					indices[0] = index; // Current key index of bone.
				}
				else
				{
					indices[1] = index; // Next key index of bone.
					return indices;
				}
			}
		}
		return indices;
	}

}