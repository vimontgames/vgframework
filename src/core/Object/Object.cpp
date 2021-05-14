#include "core/Precomp.h"
#include "Object.h"
#include "core/Kernel.h"
#include "core/File/File.h"

#include "ObjectFactory.hpp"
#include "AutoRegisterClass.hpp"

namespace vg::core
{
	//--------------------------------------------------------------------------------------
	ObjectPointer::ObjectPointer(const core::string & _name, IObject * _parent) :
		m_refCount(1)
	{
		setName(_name);
        setParent(_parent);
	}

	//--------------------------------------------------------------------------------------
	ObjectPointer::ObjectPointer() :
		m_refCount(1)
	{
		
	}

	//--------------------------------------------------------------------------------------
	ObjectPointer::~ObjectPointer()
	{

	}

    //--------------------------------------------------------------------------------------
    bool ObjectPointer::registerProperties(IObjectDescriptor & _desc)
    {
        _desc.registerPropertyHelper(ObjectPointer, m_name, "Name", IPropertyDescriptor::Flags::Hidden);
        return true;
    }

    //--------------------------------------------------------------------------------------
    void ObjectPointer::setParent(IObject * _parent)
    {
        m_parent = (ObjectPointer*)_parent;
    }

    //--------------------------------------------------------------------------------------
    IObject * ObjectPointer::getParent() const
    {
        return (IObject*)m_parent;
    }

    //--------------------------------------------------------------------------------------
    void ObjectPointer::onPropertyChanged(const IPropertyDescriptor & _prop)
    {

    }

    //--------------------------------------------------------------------------------------
    void ObjectPointer::onResourceLoaded(IResource * _resource)
    {

    }

    //--------------------------------------------------------------------------------------
    const IObjectDescriptor * ObjectPointer::getClassDesc() const
    {
        const auto * factory = Kernel::getObjectFactory();
        if (factory)
            return factory->getClassDescriptor(getClassName());
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool ObjectPointer::loadFromFile(const string & _filename)
    {
        const auto * factory = Kernel::getObjectFactory();

        string s;
        if (readFile(_filename, s, false))
            if (factory->serializeFromString(this, s))
                return true;

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ObjectPointer::saveToFile(const string & _filename)
    {
        const auto * factory = Kernel::getObjectFactory();

        string s;
        if (factory->serializeToString(s, this))
            if (writeFile(_filename, s))
                return true;

        return false;
    }

	//--------------------------------------------------------------------------------------
	void ObjectPointer::setName(const string & _name)
	{
		m_name = _name;
	}

	//--------------------------------------------------------------------------------------
	const string & ObjectPointer::getName() const
	{
		return m_name;
	}

	//--------------------------------------------------------------------------------------
	u32 ObjectPointer::addRef()
	{
		return m_refCount.fetch_add(1) + 1;
	}

	//--------------------------------------------------------------------------------------
	u32 ObjectPointer::release()
	{
		const u32 prev = m_refCount.fetch_sub(1);
		if (1 == prev)
			delete this;
		return prev-1;
	}

	//--------------------------------------------------------------------------------------
	u32 ObjectPointer::Release()
	{
		return release();
	}
}