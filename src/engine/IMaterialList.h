#pragma once

#include "core/Object/Object.h"

namespace vg::engine
{
    class IMaterialList : public core::Object
    {
    public:
        IMaterialList(const core::string & _name, core::IObject * _parent) :
            core::Object(_name, _parent)
        {

        }

        virtual ~IMaterialList() = default;

        virtual bool AddMaterial() = 0;
        virtual bool RemoveMaterial() = 0;
    };
}