#ifndef MANAGER_CLASS_H
#define MANAGER_CLASS_H

namespace ImmersiveEngine::cbs
{
	template <typename T>
	class Manager
	{
		protected:
			Manager() = default;
			~Manager() = default;

		public:
			Manager(const Manager&) = delete;
			Manager operator=(const Manager&) = delete;

			static T& getInstance()
			{
				static T instance;
				return instance;
			}
	};
}
#endif
