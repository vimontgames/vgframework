#pragma once

#include "types.hlsli"
#include "packing.hlsli"
#include "displayoptions.hlsli"

struct ViewConstants
{
    #ifndef __cplusplus
    void Load(ByteAddressBuffer _buffer, uint _offset = 0)
    {       
        m_screenSizeAndMousePos   = _buffer.Load<uint4>(_offset + 0);
        m_debugDisplay            = _buffer.Load<uint4>(_offset + 16);
        
        m_view[0]                 = _buffer.Load<float4>(_offset + 32);
        m_view[1]                 = _buffer.Load<float4>(_offset + 48);
        m_view[2]                 = _buffer.Load<float4>(_offset + 64);
        m_view[3]                 = _buffer.Load<float4>(_offset + 80);
        
        m_proj[0]                 = _buffer.Load<float4>(_offset + 96);
        m_proj[1]                 = _buffer.Load<float4>(_offset + 112);
        m_proj[2]                 = _buffer.Load<float4>(_offset + 128);
        m_proj[3]                 = _buffer.Load<float4>(_offset + 144);
    }
    #endif
    
    void            setScreenSize           (uint2 _screenSize)     { m_screenSizeAndMousePos.xy = _screenSize; }
    uint2           getScreenSize           ()                      { return m_screenSizeAndMousePos.xy; }
    
    void            setMousePos             (uint2 _mousePos)       { m_screenSizeAndMousePos.zw = _mousePos; }
    uint2           getMousePos             ()                      { return m_screenSizeAndMousePos.zw; }
    
    void            setDisplayMode          (DisplayMode _mode)     { m_debugDisplay.x = (uint)_mode; }
    DisplayMode     getDisplayMode          ()                      { return (DisplayMode)uint(m_debugDisplay.x); }
    
    void            setDisplayFlags         (DisplayFlags _flags)   { m_debugDisplay.y = (uint)_flags; }
    DisplayFlags    getDisplayFlags         ()                      { return (DisplayFlags)uint(m_debugDisplay.y); }
    
    void            setToolmodeRWBufferID   (uint _id)              { m_debugDisplay.z = (m_debugDisplay.z & ~0x0000FFFF) | _id; }
    uint            getToolmodeRWBufferID   ()                      { return m_debugDisplay.z & 0x0000FFFF; }
    
    void            setView                 (float4x4 _view)        { m_view = _view; }
    float4x4        getView                 ()                      { return m_view; }
    
    void            setProj                 (float4x4 _proj)        { m_proj = _proj; }
    float4x4        getProj                 ()                      { return m_proj; }
    
    uint4           m_screenSizeAndMousePos;
    uint4           m_debugDisplay;
    float4x4        m_view;
    float4x4        m_proj;
};