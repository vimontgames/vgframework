#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    class TestPass3D : public driver::UserPass
    {
    public:
        TestPass3D();
        ~TestPass3D();

        void setup(double _dt) override;
        void draw(driver::CommandList * _cmdList) const override;

    private:
        driver::RootSignatureHandle     m_rootSignatureHandle;
        driver::ShaderKey               m_shaderKey;
    };
}