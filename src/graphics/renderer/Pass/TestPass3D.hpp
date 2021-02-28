#include "TestPass3D.h"
#include "shaders/default/default.hlsli"
#include "Shaders/system/bindless.hlsli"

#include "graphics/renderer/View/View.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    // Setup executed once, when pass is created
    //--------------------------------------------------------------------------------------
    TestPass3D::TestPass3D()
    {
        auto * device = Device::get();

        RootSignatureDesc rsDesc;
        rsDesc.addRootConstants(ShaderStageFlags::All, 0, RootConstants3DCount);

        const RootSignatureTableDesc & bindlessTable = device->getBindlessTable()->getTableDesc();
        rsDesc.addTable(bindlessTable);

        m_rootSignatureHandle = device->addRootSignature(rsDesc);

        m_shaderKey.init("default/default.hlsl", "Forward");
    }

    //--------------------------------------------------------------------------------------
    TestPass3D::~TestPass3D()
    {
        auto * device = Device::get();
        device->removeRootSignature(m_rootSignatureHandle);
    }

    //--------------------------------------------------------------------------------------
    // Setup executed each frame, for each pass instance
    //--------------------------------------------------------------------------------------
    void TestPass3D::setup(double _dt)
    {
        writeRenderTarget(0, "Color");
    }

    //--------------------------------------------------------------------------------------
    // sx = sy/_ar
    // sy = 1.0f/tan(fov*0.5f)  
    // q = far / (near - far);
    //
    // sx   0    0     0
    //  0  sy    0     0
    //  0   0    q    -1
    //  0   0   zn*q   0
    //--------------------------------------------------------------------------------------
    float4x4 setPerspectiveProjectionRH(float _fov, float _ar, float _near, float _far)
    {
        const float sy = 1.0f / tan(_fov*0.5f);
        const float sx = sy / _ar;
        const float q = _far / (_near - _far);
        const float nq = _near * q;

        float4x4 mProj
        (
            sx, 0, 0,  0,
            0, sy, 0,  0,
            0, 0,  q, -1,
            0, 0, nq,  0
        );

        return mProj;
    }

    //--------------------------------------------------------------------------------------
    void TestPass3D::draw(CommandList * _cmdList) const
    {
        auto * renderer = Renderer::get();

        auto * device = Device::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        const float fovY = pi / 4.0f;
        const float ar = float(backbuffer.width) / float(backbuffer.height);

        RasterizerState rs(FillMode::Solid, CullMode::None);
        BlendState bs(BlendFactor::One, BlendFactor::Zero, BlendOp::Add);

        _cmdList->setRootSignature(m_rootSignatureHandle);
        _cmdList->setShader(m_shaderKey);
        _cmdList->setPrimitiveTopology(PrimitiveTopology::TriangleStrip);
        _cmdList->setRasterizerState(rs);
        _cmdList->setBlendState(bs);

        float4x4 proj = setPerspectiveProjectionRH(fovY, ar, 0.001f, 1000.0f);

        View * view = renderer->getView();
       
        float4x4 world = float4x4::identity();

        float4x4 viewProj = mul(view->getViewInvMatrix(), proj);
        float4x4 wvp = mul(world, viewProj);

        RootConstants3D root3D;

        root3D.mat = transpose(viewProj);

        _cmdList->setInlineRootConstants(&root3D, RootConstants3DCount);
        _cmdList->draw(4);
    }
}