#ifndef _VIEW__HLSLI_
#define _VIEW__HLSLI_

#include "packing.hlsli"
#include "buffer.hlsli"

#ifdef __cplusplus
namespace vg::gfx
{
using namespace vg::core;
#endif

struct ViewConstants
{
    #ifndef __cplusplus
    void Load(ByteAddressBuffer _buffer, uint _offset = 0)
    {
        screenSizeAndMousePos = LoadU4(_buffer, _offset + 0);

    }
    #endif
    
    void setScreenSize(uint2 _screenSize)
    {
        screenSizeAndMousePos.xy = _screenSize;
    }
    
    uint2 getScreenSize()
    {
        return screenSizeAndMousePos.xy;
    }
    
    void setMousePos(uint2 _mousePos)
    {
        screenSizeAndMousePos.zw = _mousePos;
    }
    
    uint2 getMousePos()
    {
        return screenSizeAndMousePos.zw;
    }
    
    uint4 screenSizeAndMousePos;
};

#ifdef __cplusplus
}
#endif

#endif // _VIEW__HLSLI_