#include "graphics/driver/FrameGraph/UserPass.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    class TestPass : public driver::UserPass
    {
    public:
        TestPass();
        ~TestPass();

        void setup() override;
        void draw(driver::CommandList * _cmdList) const override;

    private:
        driver::RootSignatureHandle m_rootSignatureHandle;
        driver::ShaderKey           m_shaderKey;
        driver::Texture *           m_texture[2] = {};
    };
}