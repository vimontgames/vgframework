#pragma once

namespace vg::core
{
	//--------------------------------------------------------------------------------------
	// Simple singleton
	//
	// usage: class MyClass : Singleton<MyClass>
	//--------------------------------------------------------------------------------------
	template <typename T> class Singleton
	{
	private:
		static inline T * s_instance = nullptr;

	protected:
		Singleton()
		{
			VG_ASSERT(nullptr == s_instance);
			s_instance = static_cast<T*>(this);
		}

		~Singleton()
		{
			VG_ASSERT(nullptr != s_instance);
			s_instance = nullptr;
		}

		Singleton(Singleton &other) = delete;
		void operator=(const Singleton &) = delete;

	public:
		static inline T * get(bool _mustExist = true) { VG_ASSERT(!_mustExist || nullptr != s_instance); return s_instance; }
	};
}