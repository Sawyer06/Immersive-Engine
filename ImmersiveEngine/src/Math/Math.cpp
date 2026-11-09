#include"Math.h"

namespace ImmersiveEngine::Math
{
	float Math::coterminal(float angle)
	{
		if (angle > 360)
		{
			return angle - 360;
		}
		else if (angle < 0)
		{
			return 360 + angle;
		}
		return angle;
	}
	Vector3 Math::coterminal(Vector3 angle)
	{
		Vector3 cotAngle = angle;
		if (angle.x > 360)
		{
			cotAngle.x -= 360;
		}
		else if (angle.x < 0)
		{
			cotAngle.x += 360;
		}
		if (angle.y > 360)
		{
			cotAngle.y -= 360;
		}
		else if (angle.y < 0)
		{
			cotAngle.y += 360;
		}
		if (angle.z > 360)
		{
			cotAngle.z -= 360;
		}
		else if (angle.z < 0)
		{
			cotAngle.z += 360;
		}
		return cotAngle;
	}
	Vector2 Math::coterminal(Vector2 angle)
	{
		Vector2 cotAngle = angle;
		if (angle.x > 360)
		{
			cotAngle.x -= 360;
		}
		else if (angle.x < 0)
		{
			cotAngle.x += 360;
		}
		if (angle.y > 360)
		{
			cotAngle.y -= 360;
		}
		else if (angle.y < 0)
		{
			cotAngle.y += 360;
		}
		return cotAngle;
	}

	float Math::signum(float x)
	{
		if (x != 0)
		{
			return x / std::abs(x);
		}
		else
		{
			return 0;
		}
	}

	glm::mat4 assimpToGLMMat4(aiMatrix4x4 assimpMat4)
	{
		glm::mat4 glmMat4;
		glmMat4[0][0] = assimpMat4.a1; glmMat4[1][0] = assimpMat4.a2; glmMat4[2][0] = assimpMat4.a3; glmMat4[3][0] = assimpMat4.a4;
		glmMat4[0][1] = assimpMat4.b1; glmMat4[1][1] = assimpMat4.b2; glmMat4[2][1] = assimpMat4.b3; glmMat4[3][1] = assimpMat4.b4;
		glmMat4[0][2] = assimpMat4.c1; glmMat4[1][2] = assimpMat4.c2; glmMat4[2][2] = assimpMat4.c3; glmMat4[3][2] = assimpMat4.c4;
		glmMat4[0][3] = assimpMat4.d1; glmMat4[1][3] = assimpMat4.d2; glmMat4[2][3] = assimpMat4.d3; glmMat4[3][3] = assimpMat4.d4;
		return glmMat4;
	}

	/// Rotate vector by a quaternion.
	Vector3 Math::operator*(const Vector3& v, const Quaternion& q)
	{	
		Quaternion qQuat = q;
		Quaternion vQuat(0.0f, v.x, v.y, v.z);
		Quaternion vPrime = qQuat * vQuat * qQuat.conjugate();
		return Vector3(vPrime.x, vPrime.y, vPrime.z);
	}
}