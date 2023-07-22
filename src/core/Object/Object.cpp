#include "core/Precomp.h"
#include "Object.h"
#include "core/Kernel.h"
#include "core/File/File.h"
#include "Property.hpp"
#include "ClassDesc.hpp"
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
    bool Object::registerProperties(IClassDesc & _desc)
    {
        _desc.registerPropertyHelperEx(Object, m_name, "Name", IProperty::Flags::Hidden);

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
    void Object::onPropertyChanged(const IProperty & _prop)
    {

    }

    //--------------------------------------------------------------------------------------
    void Object::onResourceLoaded(IResource * _resource)
    {

    }

    //--------------------------------------------------------------------------------------
    const IClassDesc * Object::getClassDesc() const
    {
        const auto * factory = Kernel::getFactory();
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
    bool Object::hasFile() const
    {
        return !m_file.empty();
    }

    //--------------------------------------------------------------------------------------
    void Object::setFile(const string & _file)
    {
        m_file = _file;
    }

    //--------------------------------------------------------------------------------------
    const string & Object::getFile() const
    {
        return m_file;
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