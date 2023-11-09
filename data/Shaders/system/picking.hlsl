#include "picking.hlsli"

//--------------------------------------------------------------------------------------
float safeLength(float2 a, float2 b)
{
    float2 diff = a - b;
    return any(0 != diff) ? length(diff) : 0;
}

//--------------------------------------------------------------------------------------
bool ProcessPicking(uint _rwBufferID, uint _offset, uint2 _inputPos, float3 _worldPos, uint2 _mousePos, uint2 _screenSize, uint4 _pickindID)
{
    if (0xFFFF != _rwBufferID)
    {        
        float dist = safeLength(_mousePos, _inputPos);
        if (all(dist <= 1) && all(_mousePos.xy >= 0) && all(_mousePos.xy < _screenSize))
        {
            PickingData pickingData;
            pickingData.m_id = _pickindID;
            pickingData.m_pos = float4(_worldPos, 1);
            
            RWByteAddressBuffer rwBuffer = getRWBuffer(_rwBufferID);
            
            rwBuffer.Store4(_offset + 0, pickingData.m_id);
            rwBuffer.Store4(_offset + 16, asuint(pickingData.m_pos));
            
            return true;
        }
    }
    
    return false;
}