#include "core/Precomp.h"
#include "Object.h"
#include "core/Kernel.h"
#include "ObjectFactory.hpp"

namespace vg::core
{
	//--------------------------------------------------------------------------------------
	Object::Object(const string & _name) :
		m_refCount(1)
	{
		setName(_name);
	}

	//--------------------------------------------------------------------------------------
	Object::Object() :
		m_refCount(1)
	{
		
	}

	//--------------------------------------------------------------------------------------
	Object::~Object()
	{

	}

    //--------------------------------------------------------------------------------------
    const IObjectDescriptor * Object::getClassDesc() const
    {
        const auto * factory = Kernel::getObjectFactory();
        if (factory)
            return factory->getClassDescriptor(getClassName());
        return nullptr;
    }

	//--------------------------------------------------------------------------------------
	void Object::setName(const string & _name)
	{
		m_name = _name;
	}

	//--------------------------------------------------------------------------------------
	const string & Object::getName() const
	{
		return m_name;
	}

	//--------------------------------------------------------------------------------------
	u32 Object::addRef()
	{
		return m_refCount.fetch_add(1) + 1;
	}

	//--------------------------------------------------------------------------------------
	u32 Object::release()
	{
		const u32 prev = m_refCount.fetch_sub(1);
		if (1 == prev)
			delete this;
		return prev-1;
	}

	//--------------------------------------------------------------------------------------
	u32 Object::Release()
	{
		return release();
	}
}