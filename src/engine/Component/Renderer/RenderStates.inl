namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE bool StencilState::isEnabled() const
    {
        return m_enable;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::u8 StencilState::getGfxStencilRef() const
    {
        return (core::u8)m_ref.getValue();
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::StencilState StencilState::getGfxStencilState() const
    {
        gfx::StencilState state;
        if (m_enable)
        {
            state.func = m_func;
            state.passOp = m_passOp;
            state.failOp = m_failOp;
            state.depthFailOp = m_depthFailOp;
        }
        return state;
    }
}