#include"Vector2.h"

#include<sstream>

namespace ImmersiveEngine::Math
{
    Vector2::Vector2() : x(0), y(0) {};

    Vector2::Vector2(float x, float y) : x(x), y(y) {};

    /// Comparison Operations
    bool Vector2::operator==(Vector2 const& b) // Equal to
    {
        return x == b.x && y == b.y;
    }
    bool Vector2::operator!=(Vector2 const& b) // Not equal to
    {
        return x != b.x && y != b.y;
    }

    // Arithmetic Operations
    Vector2 Vector2::operator=(Vector2 const& newValue) // Equals
    {
        x = newValue.x;
        y = newValue.y;
        return *this;
    }
    Vector2 Vector2::operator+(Vector2 const& addend) // Addition
    {
        return Vector2((x + addend.x), (y + addend.y));
    }
    Vector2 Vector2::operator+=(Vector2 const& addend) // Addition equal to
    {
        x += addend.x;
        y += addend.y;
        return *this;
    }
    Vector2 Vector2::operator-() // Negation
    {
        return Vector2((x * -1), (y * -1));
    }
    Vector2 Vector2::operator-(Vector2 const& subtrahend) // Subtraction
    {
        return Vector2((x - subtrahend.x), (y - subtrahend.y));
    }
    Vector2 Vector2::operator-=(Vector2 const& subtrahend) // Subtraction equal to
    {
        x -= subtrahend.x;
        y -= subtrahend.y;
        return *this;
    }
    Vector2 Vector2::operator*(float const& scalar) // Multiplication (scalar)
    {
        return Vector2((scalar * x), (scalar * y));
    }
    Vector2 Vector2::operator*=(float const& scalar) // Multiplication equal to (scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    float Vector2::operator*(Vector2 const& vecB) // Multiplication (dot product)
    {
        return (vecB.x * x) + (vecB.y * y);
    }

    std::string Vector2::toString()
    {
        std::ostringstream oss;
        oss << "(" << x << ", " << y << ")";
        return oss.str();
    }

    Vector2 Vector2::zero()
    {
        return Vector2(0, 0);
    }
}