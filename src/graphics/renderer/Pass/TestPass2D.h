#pragma once

#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    class TestPass2D : public driver::UserPass
    {
    public:
        TestPass2D();
        ~TestPass2D();

        void setup(double _dt) override;
        void draw(driver::CommandList * _cmdList) const override;

    private:
        driver::RootSignatureHandle     m_rootSignatureHandle;
        driver::ShaderKey               m_shaderKey;
        core::vector<driver::Texture *> m_texture;

        struct TestSpriteData
        {
            core::float2    pos = { 0.5f, 0.8f };
            bool            reverse = false;
        };
        TestSpriteData                  m_testSpriteData[2];
    };
}