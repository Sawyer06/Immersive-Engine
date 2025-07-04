#include"Quaternion.h"

namespace ImmersiveEngine::Math
{
	Quaternion Quaternion::identity(1, 0, 0, 0);

	Quaternion::Quaternion() :
		w(1.0f), x(0.0f), y(0.0f), z(0.0f) { }

	Quaternion::Quaternion(float w, float x, float y, float z) :
		w(w), x(x), y(y), z(z) { }

	/// Normalize between [-1, 1] q / ||q||
	void Quaternion::normalize()
	{
		float magnitude = std::sqrt(w*w + x*x + y*y + z*z);
		if (magnitude > 0.0f) // Prevent from division by 0.
		{
			w = w / magnitude;
			x = x / magnitude;
			y = y / magnitude;
			z = z / magnitude;
		}
	}
	
	/// Produces a new quaternion at a certain rotation on one axis.
	/// q = (cos(angle/2) + sin(angle/2)) * (axis)
	Quaternion Quaternion::angleAxis(float angle, ImmersiveEngine::Math::Vector3& axis)
	{
		axis.normalize();
		return Quaternion(std::cos(angle / 2), axis.x * std::sin(angle / 2), axis.y * std::sin(angle / 2), axis.z * std::sin(angle / 2));
	}

	/// Hamilton product.
	Quaternion Quaternion::operator*(const Quaternion& other)
	{
		return Quaternion(
			w * other.w - x * other.x - y * other.y - z * other.z, // W
			w * other.x + x * other.w + y * other.z - z * other.y, // X
			w * other.y - x * other.z + y * other.w + z * other.x, // Y
			w * other.z + x * other.y - y * other.x + z * other.w // Z
		);
	}

	std::string Quaternion::toString()
	{
		std::ostringstream oss;
		oss << "(" << w << ", " << x << ", " << y << ", " << z << ")";
		return oss.str();
	}
}