#ifndef MATH_CLASS_H
#define MATH_CLASS_H

#include <cmath>

#include"Vector3.h"
#include"Vector2.h"

namespace ImmersiveEngine::Math
{
	constexpr double PI = 3.1415926535897;

	float coterminal(float angle);
	Vector3 coterminal(Vector3 angle);
	Vector2 coterminal(Vector2 angle);
}

#endif
