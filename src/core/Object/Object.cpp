#include "core/Precomp.h"
#include "Object.h"
#include "core/Kernel.h"
#include "core/File/File.h"
#include "core/XML/XML.h"
#include "core/Timer/Timer.h"
#include "Property.hpp"
#include "ClassDesc.hpp"
#include "Factory.hpp"
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
        _desc.registerPropertyHelper(Object, m_name, "Name", IProperty::Flags::Hidden);

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
    bool Object::loadFromFile(const string & _filename)
    {
        const auto * factory = Kernel::getFactory();

        string s;
        if (io::readFile(_filename, s, false))
            if (factory->serializeFromString(this, s))
                return true;

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Object::saveToFile(const string & _filename) const
    {
        const auto * factory = Kernel::getFactory();

        string s;
        if (factory->serializeToString(s, this))
            if (io::writeFile(_filename, s))
                return true;

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Object::loadFromXML(const string & _XMLfilename)
    {
        const auto startLoad = Timer::getTick();
        const auto * factory = Kernel::getFactory();

        XMLDoc xmlDoc;
        if (XMLError::XML_SUCCESS == xmlDoc.LoadFile(_XMLfilename.c_str()))
        {
            XMLNode * xmlRoot = xmlDoc.FirstChild();
            if (xmlRoot != nullptr)
            {
                if (factory->serializeFromXML(this, xmlDoc))
                {
                    VG_DEBUGPRINT("[ResourceManager] XML \"%s\" loaded ... %.2f ms\n", _XMLfilename.c_str(), Timer::getEnlapsedTime(startLoad, Timer::getTick()));
                    return true;
                }
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool Object::saveToXML(const string & _XMLfilename) const
    {
        const auto * factory = Kernel::getFactory();

        XMLDoc xmlDoc;
        XMLNode * xmlRoot = xmlDoc.NewElement("Root");
        xmlDoc.InsertFirstChild(xmlRoot);

        if (factory->serializeToXML(xmlDoc, this))
            if (xmlDoc.SaveFile(_XMLfilename.c_str()))
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