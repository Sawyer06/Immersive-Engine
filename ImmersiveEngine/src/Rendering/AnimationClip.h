#ifndef ANIMATION_CLIP_CLASS_H
#define ANIMATION_CLIP_CLASS_H

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

		int boneID = -1;
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
			std::array<int, 2> getPositionKeyIndicesAtTime(int boneID, float elapsedTime);
			std::array<int, 2> getOrientationKeyIndicesAtTime(int boneID, float elapsedTime);
			std::array<int, 2> getScaleKeyIndicesAtTime(int boneID, float elapsedTime);

		public:
			AnimationClip(float duration, float ticksPerSecond);
			AnimationClip(const AnimationClip& clip);
			~AnimationClip() = default;

			Math::Vector3 getBonePositionAtTime(int boneID, float elapsedTime);
			Math::Quaternion getBoneOrientationAtTime(int boneID, float elapsedTime);
			Math::Vector3 getBoneScaleAtTime(int boneID, float elapsedTime);

			float duration;
			float ticksPerSecond;
			
			std::vector<Vector3Key> positionKeys;
			std::vector<QuaternionKey> orientationKeys;
			std::vector<Vector3Key> scaleKeys;
	};
}
#endif
