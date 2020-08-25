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
		static inline T * get() { return s_instance; }
	};

	//--------------------------------------------------------------------------------------
	// Inherit from another class and make it a singleton
	//
	// usage: class MyClass : Singleton_With_Inheritance<MyClass, MyParentClass>
	//--------------------------------------------------------------------------------------
	template <typename T, typename P> class Singleton_With_Inheritance : public P
	{
	private:
		static inline T * s_instance = nullptr;

	protected:
		Singleton_With_Inheritance()
		{
			VG_ASSERT(nullptr == s_instance);
			s_instance = static_cast<T*>(this);
		}

		~Singleton_With_Inheritance()
		{
			VG_ASSERT(nullptr != s_instance);
			s_instance = nullptr;
		}

		Singleton_With_Inheritance(Singleton_With_Inheritance &other) = delete;
		void operator=(const Singleton_With_Inheritance &) = delete;

	public:
		static inline T * get() { return s_instance; }
	};
}