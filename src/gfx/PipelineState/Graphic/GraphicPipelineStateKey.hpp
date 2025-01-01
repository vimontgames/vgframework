namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    PrimitiveType GraphicPipelineStateKey::getPrimitiveType() const
    {
        switch (m_primitiveTopology)
        {
            default:
                VG_ASSERT(false, "Unhandled PrimitiveTopology \"%s\" (%u)", asString(m_primitiveTopology).c_str(), m_primitiveTopology);

            case PrimitiveTopology::PointList:
                return PrimitiveType::Point;

            case PrimitiveTopology::LineList:
            case PrimitiveTopology::LineStrip:
                return PrimitiveType::Line;

            case PrimitiveTopology::TriangleList:
            case PrimitiveTopology::TriangleStrip:
                return PrimitiveType::Triangle;
        }
    }

    //--------------------------------------------------------------------------------------
    void GraphicPipelineStateKey::reset()
    {
        memset(this, -1, sizeof(*this));
        m_perSampleShading = false;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> inline bool isUninitializedPattern(const T & _value)
    {
        bool isUninitializedPattern = true;
        const u32 * bits = (const u32 *)&_value;
        VG_ASSERT_IS_ALIGNED(sizeof(T), 4);
        for (uint i = 0; i < sizeof(_value) >> 2; ++i)
        {
            if (0xFFFFFFFF != bits[i])
            {
                isUninitializedPattern = false;
                break;
            }
        }
        return isUninitializedPattern;
    }

    //--------------------------------------------------------------------------------------
    void GraphicPipelineStateKey::verify() const
    {
        VG_ASSERT(!isUninitializedPattern(m_rasterizerState), "GraphicPipelineStateKey uses undefined RasterizerState");
        VG_ASSERT(!isUninitializedPattern(m_depthStencilState), "GraphicPipelineStateKey uses undefined DepthStencilState");
        VG_ASSERT(!isUninitializedPattern(m_blendState), "GraphicPipelineStateKey uses undefined BlendState");
    }
}