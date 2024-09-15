#pragma once

#include "core/IPlugin.h"
#include "Audio_Consts.h"

namespace vg::audio
{
    class IAudioOptions;
    class ISound;

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
        virtual ISound *        CreateSound (const core::string & _path) = 0;
        virtual PlaySoundHandle PlaySound   (const ISound * _sound) = 0;
        virtual bool            StopSound   (const PlaySoundHandle & _handle) = 0;
	};
}