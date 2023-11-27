namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE MeshResource & MeshComponent::getMeshResource()
    {
        return m_meshResource;
    }
    
    //--------------------------------------------------------------------------------------
    VG_INLINE renderer::IMeshInstance * MeshComponent::getMeshInstance() const
    {
        return m_meshInstance;
    }
}