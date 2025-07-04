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

			void normalize();
			static Quaternion angleAxis(float angle, ImmersiveEngine::Math::Vector3& axis);
			
			Quaternion operator*(const Quaternion& other);
			
			std::string toString();

			static Quaternion identity;

	};
}
#endif
