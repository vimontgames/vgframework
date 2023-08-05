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
}