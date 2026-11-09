#ifndef ANIMATOR_CLASS_H
#define ANIMATOR_CLASS_H

#include"../Objects/GameObject.h"
#include"../Rendering/AnimationClip.h"
#include"MeshRenderer.h"

namespace ImmersiveEngine::cbs
{
	class Animator : public Component
	{
		private:
			std::shared_ptr<Rendering::AnimationClip> m_currentAnimation;

			float m_elapsedTime = 0;

			MeshRenderer* m_ownerMeshRenderer;

			std::vector<glm::mat4> m_boneTransformMats;

			void refreshBoneTransform(Rendering::ModelNode& node, glm::mat4 parentTransform);

			glm::mat4 calculateBoneTransform(Math::Vector3 position, Math::Quaternion orientation, Math::Vector3 scale);

		public:
			Animator(Object* obj);
			~Animator() = default;

			void onUpdate(float deltaTime) override;
			void refreshRigTransforms(Rendering::Shader& shaderProgram);

			void playAnimation(std::shared_ptr<Rendering::AnimationClip> anim);
			void stopAnimation();
	};
}
#endif