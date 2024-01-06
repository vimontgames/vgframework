#include "BLASCollection.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    BLASCollection::BLASCollection(BLASCollectionKey _key, BLASUpdateType _updateType) :
        m_key(_key),
        m_updateType(_updateType)
    {
        for (uint i = 0; i < core::enumCount<SurfaceType>(); ++i)
        {
            auto surfaceType = (SurfaceType)i;
            if (BLASCollection::hasSurfaceTypeBits(_key, surfaceType))
                m_BLASes[i] = new BLAS(_updateType);
            else
                m_BLASes[i] = nullptr;
        }
    }

    //--------------------------------------------------------------------------------------
    BLASCollection::~BLASCollection()
    {
        for (BLAS * blas : m_BLASes)
            VG_SAFE_RELEASE(blas);
    }

    //--------------------------------------------------------------------------------------
    bool BLASCollection::hasSurfaceTypeBits(const BLASCollectionKey _key, SurfaceType _surfaceType)
    {
        u64 key = _key;
        for (uint i = 0; i < 32; ++i)
        {
            if ((SurfaceType)(key & 0x3) == _surfaceType)
                return true;
            key >>= 2;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void BLASCollection::clear()
    {
        for (BLAS * blas : m_BLASes)
        {
            if (blas)
                blas->clear();
        }
    }

    //--------------------------------------------------------------------------------------
    void BLASCollection::update(gfx::CommandList * _cmdList)
    {
        for (BLAS * blas : m_BLASes)
        {
            if (blas)
                blas->update(_cmdList);
        }
    }
}