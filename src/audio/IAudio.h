#pragma once

#include "core/IPlugin.h"
#include "Audio_Consts.h"

namespace vg::audio
{
    class IAudioOptions;

	class IAudio : public core::IPlugin
	{
	public:
        IAudio(const core::string & _name, core::IObject * _parent) :
            core::IPlugin(_name, _parent)
        {

        }

        virtual void            Init        (const AudioCreationParams & _params, core::Singletons & _singletons) = 0;
        virtual void            Deinit      () = 0;
        virtual void            Update      (float _dt) = 0;
        virtual IAudioOptions * GetOptions  () const = 0;
	};
}