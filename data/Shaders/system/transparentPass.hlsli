#ifndef _TRANSPARENT_PASS__HLSLI_
#define _TRANSPARENT_PASS__HLSLI_

#include "types.hlsli"
#include "packing.hlsli"
#include "displaymodes.hlsli"

struct TransparentPassConstants
{
    #ifdef __cplusplus
    TransparentPassConstants()
    {
        m_data = 0;
        setLinearDepth(BINDLESS_TEXTURE_INVALID);
    }
    #else
    void Load(ByteAddressBuffer _buffer, uint _offset = 0)
    {
        m_data = _buffer.Load<TransparentPassConstants>(_offset).m_data;
    }
    #endif

    uint   m_data;    
    
    void    setLinearDepth  (uint _linearZ) { m_data = packUint16low(m_data, _linearZ); }
    uint    getLinearDepth  ()              { return unpackUint16low(m_data); }
};

#endif // _TRANSPARENT_PASS__HLSLI_