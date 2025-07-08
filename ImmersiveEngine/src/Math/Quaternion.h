#ifndef QUATERNION_CLASS_H
#define QUATERNION_CLASS_H

#include<cmath>
#include<string>
#include<sstream>

#include"Vector3.h"

namespace ImmersiveEngine::Math
{
	class Quaternion
	{
		public:
			Quaternion();
			Quaternion(float w, float x, float y, float z);

			float w, x, y, z;

			Quaternion normalize() const;
			static Quaternion angleAxis(float angle, Vector3& axis);
			static Quaternion rotationTo(Vector3 start, Vector3 target);
			Quaternion conjugate() const;
			
			bool operator==(Quaternion const& b);
			bool operator!=(Quaternion const& b);

			Quaternion operator=(Quaternion const& newValue);
			Quaternion operator*(Quaternion const& quatB);
			Quaternion operator*=(Quaternion const& quatB);
			
			std::string toString();

			static Quaternion identity;
	};
}
#endif
