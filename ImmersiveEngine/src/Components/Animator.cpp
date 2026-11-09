#include"Animator.h"

namespace ImmersiveEngine::cbs
{
	Animator::Animator(Object* obj) : Component(obj), m_boneTransformMats(MAX_BONES, glm::mat4(1.0f))
	{
		auto owner = Component::getOwner();
		m_ownerMeshRenderer = owner->getComponent<MeshRenderer>();
		if (m_ownerMeshRenderer == nullptr)
		{
			MISORDERED_ERROR;
			return;
		}
	}

	void Animator::onUpdate(float deltaTime)
	{
		if (m_currentAnimation)
		{
			m_elapsedTime += m_currentAnimation->ticksPerSecond * deltaTime;
			m_elapsedTime = fmod(m_elapsedTime, m_currentAnimation->duration);
		}
	}

	void Animator::refreshRigTransforms(Rendering::Shader& shaderProgram)
	{
		if (!m_currentAnimation)
			return;

		if (auto model = m_ownerMeshRenderer->getModel())
		{
			refreshBoneTransform(model->getRoot(), glm::mat4(1.0));
			for (int i = 0; i < m_boneTransformMats.size(); i++)
			{
				shaderProgram.setMat4("boneTransformMats[" + std::to_string(i) + "]", m_boneTransformMats[i]);
			}
		}
	}

	/// Refresh all local transforms of bone nodes and convert them to global space.
	void Animator::refreshBoneTransform(Rendering::ModelNode& node, glm::mat4 parentTransform)
	{
		//glm::mat4 globalTransform = parentTransform * node.localTransform;

		if (node.boneID > -1) // Node is associated with bone, get animated transform at current time.
		{
			Math::Vector3 position;
			if (m_currentAnimation->positionKeys[node.boneID].size() > 0)
				position = m_currentAnimation->getBonePositionAtTime(node.boneID, m_elapsedTime);

			Math::Quaternion orientation = Math::Quaternion::identity;
			if (m_currentAnimation->orientationKeys[node.boneID].size() > 0)
				orientation = m_currentAnimation->getBoneOrientationAtTime(node.boneID, m_elapsedTime);

			Math::Vector3 scale;
			if (m_currentAnimation->scaleKeys[node.boneID].size() > 0)
				scale = m_currentAnimation->getBoneScaleAtTime(node.boneID, m_elapsedTime);

			node.localTransform = calculateBoneTransform(position, orientation, scale);
		}

		glm::mat4 globalTransform = parentTransform * node.localTransform;

		if (node.boneID > -1)
			m_boneTransformMats[node.boneID] = globalTransform * node.localOffset;

		// Refresh transforms of all child nodes.
		for (auto& child : node.children)
		{
			refreshBoneTransform(child, globalTransform);
		}
	}

	glm::mat4 Animator::calculateBoneTransform(Math::Vector3 position, Math::Quaternion orientation, Math::Vector3 scale)
	{
		glm::mat4 mat4Position = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z));
		
		glm::mat4 mat4Orientation = glm::mat4_cast(glm::quat(orientation.w, orientation.x, orientation.y, orientation.z));

		glm::mat4 mat4Scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

		return mat4Position * mat4Orientation * mat4Scale;
	}

	void Animator::playAnimation(std::shared_ptr<Rendering::AnimationClip> anim)
	{
		m_elapsedTime = 0;
		m_currentAnimation = anim;
	}
	void Animator::stopAnimation()
	{
		m_currentAnimation = nullptr;
	}
}