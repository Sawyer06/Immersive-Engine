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

	/// Rotate vector by a quaternion.
	Vector3 Math::operator*(const Vector3& v, const Quaternion& q)
	{	
		Quaternion qQuat = q;
		Quaternion vQuat(0.0f, v.x, v.y, v.z);
		Quaternion vPrime = qQuat * vQuat * qQuat.conjugate();
		return Vector3(vPrime.x, vPrime.y, vPrime.z);
	}
}