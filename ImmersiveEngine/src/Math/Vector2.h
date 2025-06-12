#ifndef VECTOR2_CLASS_H
#define VECTOR2_CLASS_H

#include<string>

namespace ImmersiveEngine::Math
{
	class Vector2
	{
		public:
            Vector2();
            Vector2(float x, float y);
			
            float x, y;

            bool operator==(Vector2 const& b);
            bool operator!=(Vector2 const& b);

            Vector2 operator=(Vector2 const& newValue);
            Vector2 operator+(Vector2 const& addend);
            Vector2 operator+=(Vector2 const& addend);
            Vector2 operator-();
            Vector2 operator-(Vector2 const& subtrahend);
            Vector2 operator-=(Vector2 const& subtrahend);
            Vector2 operator*(float const& scalar);
            Vector2 operator*=(float const& scalar);
            float operator*(Vector2 const& vecB);

            std::string toString();

            static Vector2 zero();
	};
}

#endif
