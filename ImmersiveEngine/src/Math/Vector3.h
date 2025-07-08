#ifndef VECTOR3_CLASS_H
#define VECTOR3_CLASS_H

#include<string>
#include<sstream>
#include<cmath>

namespace ImmersiveEngine::Math
{
	class Vector3
	{
		public:
            Vector3();
            Vector3(float x, float y, float z);
			
            float x, y, z;

            Vector3 normalize();

            bool operator==(const Vector3& b);
            bool operator!=(const Vector3& b);

            Vector3 operator=(const Vector3& newValue);
            Vector3 operator+(const Vector3& addend);
            Vector3 operator+=(const Vector3& addend);
            Vector3 operator-();
            Vector3 operator-(const Vector3& subtrahend);
            Vector3 operator-=(const Vector3& subtrahend);
            Vector3 operator*(const float& scalar);
            Vector3 operator*=(const float& scalar);
            float operator*(const Vector3& vecB);

            std::string toString();

            static Vector3 zero;
            static Vector3 up;
            static Vector3 forward;
            static Vector3 right;
	};
}

#endif
