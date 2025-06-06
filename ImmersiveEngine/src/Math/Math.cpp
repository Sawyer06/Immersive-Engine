#include"Math.h"

namespace Engine::Math
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
}