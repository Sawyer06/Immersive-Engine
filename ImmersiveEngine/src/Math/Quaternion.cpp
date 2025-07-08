#include"Quaternion.h"

namespace ImmersiveEngine::Math
{
	Quaternion Quaternion::identity(1, 0, 0, 0);

	Quaternion::Quaternion() :
		w(1.0f), x(0.0f), y(0.0f), z(0.0f) { }

	Quaternion::Quaternion(float w, float x, float y, float z) :
		w(w), x(x), y(y), z(z) { }

	/// Normalize between [-1, 1] q / ||q||
	Quaternion Quaternion::normalize() const
	{
		float magnitude = std::sqrt(w*w + x*x + y*y + z*z);
		float nW = w; float nX = x; float nY = y; float nZ = z;
		if (magnitude > 0.0f) // Prevent from division by 0.
		{
			nW = w / magnitude;
			nX = x / magnitude;
			nY = y / magnitude;
			nZ = z / magnitude;
		}
		return Quaternion(nW, nX, nY, nZ);
	}
	
	/// Produces a new quaternion at a certain rotation on one axis.
	/// q = (cos(angle/2) + sin(angle/2)) * (axis)
	Quaternion Quaternion::angleAxis(float angle, Vector3& axis)
	{
		Vector3 normalizedAxis = axis.normalize();
		return Quaternion(std::cos(angle / 2), normalizedAxis.x * std::sin(angle / 2), normalizedAxis.y * std::sin(angle / 2), normalizedAxis.z * std::sin(angle / 2));
	}

	/// Produces a quaternion whose forward is pointing at the given position.
	Quaternion Quaternion::rotationTo(Vector3 start, Vector3 target)
	{
		Vector3 dir = (target - start).normalize();

		// arctan(opposite/adjacent) = theta
		float pitch = std::atan2(dir.y, std::sqrt(dir.x * dir.x + dir.z * dir.z)); // Calculate the length of the 3rd side (c) using pythagorean theorem (a^2+b^2=c^2). 
		float yaw = std::atan2(dir.x, dir.z);
		
		Quaternion rot = Quaternion::angleAxis(yaw, Vector3::up) * Quaternion::angleAxis(-pitch, Vector3::right);
		return rot.normalize();
	}

	/// Returns the conjugate of this quaternion.
	Quaternion Quaternion::conjugate() const
	{
		return Quaternion(w, -x, -y, -z);
	}

	/// Comparison Operations
	bool Quaternion::operator==(Quaternion const& b) // Equal to
	{
		return w == b.w && x == b.x && y == b.y && z == b.z;
	}
	bool Quaternion::operator!=(Quaternion const& b) // Not equal to
	{
		return w != b.w && x != b.x && y != b.y && z != b.z;
	}
	
	/// Arithmetic Operations
	Quaternion Quaternion::operator=(Quaternion const& newValue) // Set
	{
		w = newValue.w;
		x = newValue.x;
		y = newValue.y;
		z = newValue.z;

		return *this;
	}
	Quaternion Quaternion::operator*(Quaternion const& quatB) // Hamilton product
	{
		return Quaternion(
			w * quatB.w - x * quatB.x - y * quatB.y - z * quatB.z, // W
			w * quatB.x + x * quatB.w + y * quatB.z - z * quatB.y, // X
			w * quatB.y - x * quatB.z + y * quatB.w + z * quatB.x, // Y
			w * quatB.z + x * quatB.y - y * quatB.x + z * quatB.w // Z
		);
	}
	Quaternion Quaternion::operator*=(Quaternion const& quatB) // Hamilton product equal to
	{
		w = w * quatB.w - x * quatB.x - y * quatB.y - z * quatB.z; // W
		x = w * quatB.x + x * quatB.w + y * quatB.z - z * quatB.y; // X
		y = w * quatB.y - x * quatB.z + y * quatB.w + z * quatB.x; // Y
		z = w * quatB.z + x * quatB.y - y * quatB.x + z * quatB.w; // Z

		return *this;
	}

	std::string Quaternion::toString()
	{
		std::ostringstream oss;
		oss << "(" << w << ", " << x << ", " << y << ", " << z << ")";
		return oss.str();
	}
}