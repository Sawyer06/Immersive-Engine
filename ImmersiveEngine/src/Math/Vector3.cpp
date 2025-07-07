#include"Vector3.h"

namespace ImmersiveEngine::Math
{
    Vector3 Vector3::zero(0, 0, 0);
    Vector3 Vector3::up(0, 1, 0);
    Vector3 Vector3::forward(0, 0, 1);
    Vector3 Vector3::right(1, 0, 0);

    Vector3::Vector3() : x(0), y(0), z(0) {};

    Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {};

    /// Normalize between [-1, 1] v / ||v||
    Vector3 Vector3::normalize()
    {
        float magnitude = std::sqrt(x*x + y*y + z*z);
        if (magnitude > 0.0f)
        {
            x = x / magnitude;
            y = y / magnitude;
            z = z / magnitude;
        }
        return { x, y, z };
    }

    /// Comparison Operations
    bool Vector3::operator==(Vector3 const& b) // Equal to
    {
        return x == b.x && y == b.y && z == b.z;
    }
    bool Vector3::operator!=(Vector3 const& b) // Not equal to
    {
        return x != b.x && y != b.y && z != b.z;
    }

    // Arithmetic Operations
    Vector3 Vector3::operator=(Vector3 const& newValue) // Equals
    {
        x = newValue.x;
        y = newValue.y;
        z = newValue.z;
        return *this;
    }
    Vector3 Vector3::operator+(Vector3 const& addend) // Addition
    {
        return Vector3((x + addend.x), (y + addend.y), (z + addend.z));
    }
    Vector3 Vector3::operator+=(Vector3 const& addend) // Addition equal to
    {
        x += addend.x;
        y += addend.y;
        z += addend.z;
        return *this;
    }
    Vector3 Vector3::operator-() // Negation
    {
        return Vector3((x * -1), (y * -1), (z * -1));
    }
    Vector3 Vector3::operator-(Vector3 const& subtrahend) // Subtraction
    {
        return Vector3((x - subtrahend.x), (y - subtrahend.y), (z - subtrahend.z));
    }
    Vector3 Vector3::operator-=(Vector3 const& subtrahend) // Subtraction equal to
    {
        x -= subtrahend.x;
        y -= subtrahend.y;
        z -= subtrahend.z;
        return *this;
    }
    Vector3 Vector3::operator*(float const& scalar) // Multiplication (scalar)
    {
        return Vector3((scalar * x), (scalar * y), (scalar * z));
    }
    Vector3 Vector3::operator*=(float const& scalar) // Multiplication equal to (scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    float Vector3::operator*(Vector3 const& vecB) // Multiplication (dot product)
    {
        return (vecB.x * x) + (vecB.y * y) + (vecB.z * z);
    }

    std::string Vector3::toString()
    {
        std::ostringstream oss;
        oss << "(" << x << ", " << y << ", " << z << ")";
        return oss.str();
    }
}