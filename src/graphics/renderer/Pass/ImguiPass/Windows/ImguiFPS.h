#pragma once

#include "ImguiEditor.h"

namespace vg::graphics::renderer
{
    class ImguiFPS : public ImguiEditor
    {
    public:
        ImguiFPS(const string & _name, Flags _flags) :
            ImguiEditor(_name, _flags)
        {
        }

        virtual void    update(double _dt) final;
        virtual void    display() final;

    private:
        core::uint      m_captureFrameCounter = 0;
        double          m_accum = 0.0f;
        core::uint      m_frame = 0;
        float           m_dt = 0.0f;
        float           m_fps = 0.0f;
    };
}