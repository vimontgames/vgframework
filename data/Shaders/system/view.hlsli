#pragma once

#include "types.hlsli"
#include "packing.hlsli"
#include "options.hlsli"

struct ViewConstants
{
    #ifndef __cplusplus
    void Load(ByteAddressBuffer _buffer, uint _offset = 0)
    {       
        m_screenSizeAndMousePos   = _buffer.Load<uint4>(_offset);   _offset += sizeof(uint4);
        m_debugDisplay            = _buffer.Load<uint4>(_offset);   _offset += sizeof(uint4);
        m_camera                  = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_rayTracing              = _buffer.Load<uint4>(_offset);   _offset += sizeof(uint4);
                                                                  
        m_view[0]                 = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_view[1]                 = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_view[2]                 = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_view[3]                 = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
                                                                    
        m_proj[0]                 = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_proj[1]                 = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_proj[2]                 = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_proj[3]                 = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);

        m_viewInv[0]              = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_viewInv[1]              = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_viewInv[2]              = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_viewInv[3]              = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
                                                                 
        m_projInv[0]              = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_projInv[1]              = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_projInv[2]              = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_projInv[3]              = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
    }
    #endif
    
    // m_screenSizeAndMousePos
    void            setScreenSize           (uint2 _screenSize)     { m_screenSizeAndMousePos.xy = _screenSize; }
    uint2           getScreenSize           ()                      { return m_screenSizeAndMousePos.xy; }
    
    void            setMousePos             (uint2 _mousePos)       { m_screenSizeAndMousePos.zw = _mousePos; }
    uint2           getMousePos             ()                      { return m_screenSizeAndMousePos.zw; }
    
    // m_debugDisplay.x
    // DisplayMode      : 8
    // RayTracingMode   : 8
    // PostProcessMode  : 8
    void            setDisplayMode          (DisplayMode _mode)         { m_debugDisplay.x = (m_debugDisplay.x & ~0x000000FF) | ((uint)_mode & 0xFF)<<0; }
    DisplayMode     getDisplayMode          ()                          { return (DisplayMode)(uint(m_debugDisplay.x) & 0x000000FF); }

    void            setRayTracingMode       (RayTracingMode _mode)      { m_debugDisplay.x = (m_debugDisplay.x & ~0x0000FF00) | ((uint)_mode & 0xFF)<<8; }
    RayTracingMode  getRayTracingMode       ()                          { return (RayTracingMode)(uint(m_debugDisplay.x)>>8 & 0xFF); }

    void            setPostProcessMode      (PostProcessMode _mode)     { m_debugDisplay.x = (m_debugDisplay.x & ~0x00FF0000) | ((uint)_mode & 0xFF)<<16; }
    PostProcessMode getPostProcessMode      ()                          { return (PostProcessMode)(uint(m_debugDisplay.x)>>16 & 0xFF); }
    
    void            setDisplayFlags         (DisplayFlags _flags)       { m_debugDisplay.y = (uint)_flags; }
    DisplayFlags    getDisplayFlags         ()                          { return (DisplayFlags)uint(m_debugDisplay.y); }
    
    void            setToolmodeRWBufferID   (uint _id)                  { m_debugDisplay.z = (m_debugDisplay.z & ~0x0000FFFF) | _id; }
    uint            getToolmodeRWBufferID   ()                          { return m_debugDisplay.z & 0x0000FFFF; }

    // m_camera
    void            setCameraNearFar        (float2 _nearFar)           { m_camera.xy = _nearFar; }
    void            setCameraFieldOfView    (float _fov)                { m_camera.z = _fov; }
    void            setCameraAspectRatio    (float _ar)                 { m_camera.w = _ar; }

    float2          getCameraNearFar           ()                       { return m_camera.xy; }
    float           getCameraFieldOfView    ()                          { return m_camera.z; }
    float           getCameraAspectRatio    ()                          { return m_camera.w; }
    
    void            setView                 (float4x4 _view)            { m_view = _view; }
    float4x4        getView                 ()                          { return m_view; }

    void            setViewInv              (float4x4 _viewInv)         { m_viewInv = _viewInv; }
    float4x4        getViewInv              ()                          { return m_viewInv; }
    
    void            setProj                 (float4x4 _proj)            { m_proj = _proj; }
    float4x4        getProj                 ()                          { return m_proj; }

    void            setProjInv              (float4x4 _projInv)         { m_projInv = _projInv; }
    float4x4        getProjInv              ()                          { return m_projInv; }

    void            setTLASHandle           (uint _value)               { m_rayTracing.x = (m_rayTracing.x & ~0x0000FFFFUL) | (_value & 0xFFFF); }
    uint            getTLASHandle           ()                          { return 0xFFFF & m_rayTracing.x; }
    
    uint4           m_screenSizeAndMousePos;    // { size.x, size.y, mouse.x, mouse.y }
    uint4           m_debugDisplay;             // 0x00FFFFFF 0xFFFFFFFF 0x0000FFFF 0x00000000
    float4          m_camera;                   // { near, far, fov, ar }
    uint4           m_rayTracing;               // 0x0000FFFF 0x00000000 0x00000000 0x00000000
    float4x4        m_view;
    float4x4        m_proj;
    float4x4        m_viewInv;
    float4x4        m_projInv;

    float getLinearDepth(float _zBufferDepth)
    {
        float n = getCameraNearFar().x;
        float f = getCameraNearFar().y;
        return (2.0f * n) / (f + n - _zBufferDepth * (f - n));
    }

    float3 getViewPos(float2 _screenPos, float _zBufferDepth)
    {
        float4 clipPos = float4(float2(_screenPos.x, 1-_screenPos.y) * 2.0f - 1.0f, _zBufferDepth, 1.0f);
        float4 viewPos = mul(clipPos, getProjInv());
        return viewPos.xyz / viewPos.w;
    }

    float3 getWorldPos(float2 _screenPos, float _zBufferDepth)
    {
        float3 viewPos = getViewPos(_screenPos, _zBufferDepth);
        return mul(float4(viewPos.xyz,1.0f), getViewInv()).xyz;
    }
};