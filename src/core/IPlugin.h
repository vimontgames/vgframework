#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
	class IPlugin : public core::Object
	{
	public:
        IPlugin(const core::string & _name, core::IObject * _parent) :
            core::Object(_name, _parent)
        {

        }

		virtual			~IPlugin			() {}

        virtual bool    RegisterClasses		() = 0;
		virtual bool    UnregisterClasses	() = 0;

		virtual void	Update				(float _dt) {};

		virtual void	CreateResources		() {};
		virtual void	ReleaseResources	() {};
	};
}