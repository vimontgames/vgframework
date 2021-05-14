#pragma once

#include "Object/Object.h"

namespace vg::core
{
    class IResource : public ObjectPointer
    {
    public:
        virtual ~IResource() {}
        virtual IObject * get() const = 0;
        virtual const string & getPath() const = 0;
        virtual bool setPath(const string & _path) = 0;
        virtual const vector<string> getExtensions() const = 0;
        virtual void onPathChanged(IObject * _owner, const string & _oldPath, const string & _newPath) = 0;
        virtual bool loadResource(const core::string & _path, core::IObject * _owner) = 0;
		virtual bool set(core::IObject * _object) = 0;
    };
}