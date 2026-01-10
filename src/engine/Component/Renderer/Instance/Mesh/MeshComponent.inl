namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const MeshResource & MeshComponent::getMeshResource() const
    {
        return m_meshResource;
    }

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