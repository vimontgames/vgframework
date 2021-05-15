#include "core/Precomp.h"
#include "Object.h"
#include "core/Kernel.h"
#include "core/File/File.h"

#include "ObjectFactory.hpp"
#include "AutoRegisterClass.hpp"

namespace vg::core
{
	//--------------------------------------------------------------------------------------
	Object::Object(const core::string & _name, IObject * _parent) :
		m_refCount(1)
	{
		setName(_name);
        setParent(_parent);
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
    bool Object::registerProperties(IObjectDescriptor & _desc)
    {
        _desc.registerPropertyHelper(Object, m_name, "Name", IPropertyDescriptor::Flags::Hidden);
        return true;
    }

    //--------------------------------------------------------------------------------------
    void Object::setParent(IObject * _parent)
    {
        m_parent = (Object*)_parent;
    }

    //--------------------------------------------------------------------------------------
    IObject * Object::getParent() const
    {
        return (IObject*)m_parent;
    }

    //--------------------------------------------------------------------------------------
    void Object::onPropertyChanged(const IPropertyDescriptor & _prop)
    {

    }

    //--------------------------------------------------------------------------------------
    void Object::onResourceLoaded(IResource * _resource)
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
    bool Object::loadFromFile(const string & _filename)
    {
        const auto * factory = Kernel::getObjectFactory();

        string s;
        if (readFile(_filename, s, false))
            if (factory->serializeFromString(this, s))
                return true;

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Object::saveToFile(const string & _filename)
    {
        const auto * factory = Kernel::getObjectFactory();

        string s;
        if (factory->serializeToString(s, this))
            if (writeFile(_filename, s))
                return true;

        return false;
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