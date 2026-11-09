#ifndef ANIMATION_CLIP_CLASS_H
#define ANIMATION_CLIP_CLASS_H

#include<iostream>
#include<vector>
#include<array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"../Application/Resource.h"
#include"../Math/Vector3.h"
#include"../Math/Quaternion.h"

namespace ImmersiveEngine::Rendering
{
	struct Key
	{
		float timeStamp;
	};

	struct Vector3Key : Key
	{
		Math::Vector3 value;
	};

	struct QuaternionKey : Key
	{
		Math::Quaternion value;
	};

	class AnimationClip : public Resource
	{
		private:
			int getPositionKeyIndexAtTime(int boneID, float elapsedTime);
			int getOrientationKeyIndexAtTime(int boneID, float elapsedTime);
			int getScaleKeyIndexAtTime(int boneID, float elapsedTime);

		public:
			AnimationClip(float duration, float ticksPerSecond);
			AnimationClip(const AnimationClip& clip);
			~AnimationClip() = default;

			Math::Vector3 getBonePositionAtTime(int boneID, float elapsedTime);
			Math::Quaternion getBoneOrientationAtTime(int boneID, float elapsedTime);
			Math::Vector3 getBoneScaleAtTime(int boneID, float elapsedTime);

			float duration;
			float ticksPerSecond;
			
			// Grid of keys where row number is the associated bone ID for those keys.
			std::vector<std::vector<Vector3Key>> positionKeys;
			std::vector<std::vector<QuaternionKey>> orientationKeys;
			std::vector<std::vector<Vector3Key>> scaleKeys;
	};
}
#endif
