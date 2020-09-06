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
        driver::Shader *            m_vs = nullptr;
        driver::Shader *            m_ps = nullptr;
    };
}