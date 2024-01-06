#pragma once
#include "core/Object/Object.h"
#include "gfx/Raytracing/RayTracing_consts.h"

namespace vg::gfx
{
    class BLAS;
    class MaterialModel;

    //--------------------------------------------------------------------------------------
    // BLASCollectionKey will depend on the surface type of the materials used
    // If the key remains the same, then the BLAS can be updated
    // If the key changes, then the BLAS has to be recreated
    //--------------------------------------------------------------------------------------
    using BLASCollectionKey = core::u64;

    //--------------------------------------------------------------------------------------
    // A collection of BLAS for a geometry made of several batchs
    // Batches are added to the corresponding opaque/alphatest/transparent BLAS vector 
    //--------------------------------------------------------------------------------------
    class BLASCollection : public core::Object
    {
    public:
        BLASCollection(BLASCollectionKey _key, BLASUpdateType _updateType);
        ~BLASCollection();

        VG_INLINE BLAS *            getBLAS (SurfaceType _surfaceType) const                                                    { return m_BLASes[core::asInteger(_surfaceType)]; }
        VG_INLINE BLASCollectionKey getKey  () const                                                                            { return m_key; }

        static VG_INLINE void       setSurfaceTypeBits(BLASCollectionKey & _key, core::uint _index, SurfaceType _surfaceType)   { _key |= ((core::u64)_surfaceType) << (_index<<1); }
        static bool                 hasSurfaceTypeBits(const BLASCollectionKey _key, SurfaceType _surfaceType);

        void clear                  ();
        void update                 (gfx::CommandList * _cmdList);

        BLASCollectionKey           m_key;
        BLASUpdateType              m_updateType;
        BLAS *                      m_BLASes[core::enumCount<SurfaceType>()];
    };
}