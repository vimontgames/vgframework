#include "picking.hlsli" 
#include "debug.hlsli"

//--------------------------------------------------------------------------------------
bool ProcessPicking(uint _rwBufferID, uint _offset, uint2 _inputPos, float _depth, float3 _worldPos, uint2 _mousePos, uint2 _screenSize, uint4 _pickindID)
{
    if (0xFFFF != _rwBufferID)
    {        
        if (all(_mousePos.xy == _inputPos.xy) && all(_mousePos.xy < _screenSize))
        {
            // Get picking RWBuffer
            RWByteAddressBuffer rwBuffer = getRWBuffer(_rwBufferID);
            
            // Get index to store picking hit
            uint index;
            rwBuffer.InterlockedAdd(_offset, 1, index);
            
            // Store picking hit
            if (index < PICKING_MAX_HITS)
            {
                PickingHit hit;
                hit.m_id = _pickindID;
                hit.m_pos = float4(_worldPos.xyz, _depth);
                        
                rwBuffer.Store4(_offset + index * sizeof(PickingHit) + 16, hit.m_id);
                rwBuffer.Store4(_offset + index * sizeof(PickingHit) + 32, asuint(hit.m_pos));
            }
            
            return true;
        }
    }
    
    return false;
}