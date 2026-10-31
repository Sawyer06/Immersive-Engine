#ifndef ANIMATOR_CLASS_H
#define ANIMATOR_CLASS_H

#include"../Objects/GameObject.h"

namespace ImmersiveEngine::cbs
{
	class Animator : public Component
	{
		private:


		public:
			Animator(Object* obj);
			~Animator() = default;
	};
}
#endif