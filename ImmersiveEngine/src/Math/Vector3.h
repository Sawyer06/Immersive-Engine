#ifndef VECTOR3_CLASS_H
#define VECTOR3_CLASS_H

#include<string>

namespace ImmersiveEngine::Math
{
	class Vector3
	{
		public:
            Vector3();
            Vector3(float x, float y, float z);
			
            float x, y, z;

            bool operator==(Vector3 const& b);
            bool operator!=(Vector3 const& b);

            Vector3 operator=(Vector3 const& newValue);
            Vector3 operator+(Vector3 const& addend);
            Vector3 operator+=(Vector3 const& addend);
            Vector3 operator-();
            Vector3 operator-(Vector3 const& subtrahend);
            Vector3 operator-=(Vector3 const& subtrahend);
            Vector3 operator*(float const& scalar);
            Vector3 operator*=(float const& scalar);
            float operator*(Vector3 const& vecB);

            std::string toString();

            static Vector3 zero();
	};
}

#endif
