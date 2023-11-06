#ifndef _VIEW__HLSLI_
#define _VIEW__HLSLI_

#pragma once

#include "types.hlsli"
#include "packing.hlsli"
#include "buffer.hlsli"
#include "displayoptions.hlsli"

struct ViewConstants
{
    #ifndef __cplusplus
    void Load(ByteAddressBuffer _buffer, uint _offset = 0)
    {
        screenSizeAndMousePos   = LoadU4(_buffer, _offset + 0);
        debugDisplay            = LoadU4(_buffer, _offset + 4);

    }
    #endif
    
    void            setScreenSize           (uint2 _screenSize)     { screenSizeAndMousePos.xy = _screenSize; }
    uint2           getScreenSize           ()                      { return screenSizeAndMousePos.xy; }
    
    void            setMousePos             (uint2 _mousePos)       { screenSizeAndMousePos.zw = _mousePos; }
    uint2           getMousePos             ()                      { return screenSizeAndMousePos.zw; }
    
    void            setDisplayMode          (DisplayMode _mode)     { debugDisplay.x = (uint)_mode; }
    DisplayMode     getDisplayMode          ()                      { return (DisplayMode)uint(debugDisplay.x); }
    
    void            setDisplayFlags         (DisplayFlags _flags)   { debugDisplay.y = (uint)_flags; }
    DisplayFlags    getDisplayFlags         ()                      { return (DisplayFlags)uint(debugDisplay.y); }
    
    void            setToolmodeRWBufferID   (uint _id)              { debugDisplay.z = (debugDisplay.z & ~0x0000FFFF) | _id; }
    uint            getToolmodeRWBufferID   ()                      { return debugDisplay.z & 0x0000FFFF; }
    
    uint4           screenSizeAndMousePos;
    uint4           debugDisplay;
};

#endif // _VIEW__HLSLI_