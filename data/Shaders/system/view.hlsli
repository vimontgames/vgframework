#ifndef _VIEW__HLSLI_
#define _VIEW__HLSLI_

#include "types.hlsli"
#include "packing.hlsli"
#include "displaymodes.hlsli"
#include "outlinemask.hlsli"

struct ViewConstants
{
    #ifdef __cplusplus
    ViewConstants()
    {
        m_screenSizeAndMousePos = (uint4)0;
        m_debugDisplay          = (uint4)0;
        m_camera                = (float4)0.0;
        m_rayTracing            = (uint4)0;

        m_view[0]               = (float4)0.0;
        m_view[1]               = (float4)0.0;
        m_view[2]               = (float4)0.0;
        m_view[3]               = (float4)0.0;

        m_proj[0]               = (float4)0.0;
        m_proj[1]               = (float4)0.0;
        m_proj[2]               = (float4)0.0;
        m_proj[3]               = (float4)0.0;

        m_viewInv[0]            = (float4)0.0;
        m_viewInv[1]            = (float4)0.0;
        m_viewInv[2]            = (float4)0.0;
        m_viewInv[3]            = (float4)0.0;

        m_projInv[0]            = (float4)0.0;
        m_projInv[1]            = (float4)0.0;
        m_projInv[2]            = (float4)0.0;
        m_projInv[3]            = (float4)0.0;

        m_environmentColor      = (float4)0.0f;
        m_pbr                   = (float4)0.0f;
        m_textures              = (uint4)0;
    
        for (uint i = 0; i < OUTLINE_MASK_CATEGORIES_MAX; ++i)
        {
            m_outlineCategory[i].zPassOutlineColor = (float4)0.0f;
            m_outlineCategory[i].zFailOutlineColor = (float4)0.0f;
            m_outlineCategory[i].zFailFillColor = (float4)0.0f;
        }
    }
    #else 
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

        m_environmentColor        = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_pbr                     = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        m_textures                = _buffer.Load<uint4>(_offset);   _offset += sizeof(uint4);

        m_lens                    = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        
        // TODO: use 'LoadOutlineColors' method instead to save VGPRs
        for (uint i = 0; i < OUTLINE_MASK_CATEGORIES_MAX; ++i)
        {
            m_outlineCategory[i].zPassOutlineColor = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
            m_outlineCategory[i].zFailOutlineColor = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
            m_outlineCategory[i].zFailFillColor = _buffer.Load<float4>(_offset);  _offset += sizeof(float4);
        }
    }
    #endif

    uint4                       m_screenSizeAndMousePos;    // { size.x, size.y, mouse.x, mouse.y }
    uint4                       m_debugDisplay;             // 0xFFFFFFFF 0x00000000 0x0000FFFF 0x00000000
    float4                      m_camera;                   // { near, far, fov, ar }
    uint4                       m_rayTracing;               // 0x0000FFFF 0x00000000 0x00000000 0x00000000
    float4x4                    m_view;
    float4x4                    m_proj;
    float4x4                    m_viewInv;
    float4x4                    m_projInv;
    float4                      m_environmentColor;
    float4                      m_pbr;
    uint4                       m_textures;                 // .x: EnvMap (low) + specularBRDF (high) | .y: IrradianceEnvmap (low) + SpecularReflectionMap (hight)
    float4                      m_lens;                     // .x: focal length, .y:aperture, .z:focus distance
    OutlineCategoryConstants    m_outlineCategory[OUTLINE_MASK_CATEGORIES_MAX];
    
    // Screen and mouse constants
    void            setScreenSize           (uint2 _screenSize)                 { m_screenSizeAndMousePos.xy = _screenSize; }
    uint2           getScreenSize           ()                                  { return m_screenSizeAndMousePos.xy; }
    
    void            setMousePos             (uint2 _mousePos)                   { m_screenSizeAndMousePos.zw = _mousePos; }
    uint2           getMousePos             ()                                  { return m_screenSizeAndMousePos.zw; }
    
    // Debug display constants
    void            setDisplayMode          (DisplayMode _mode)                 { m_debugDisplay.x = packUint16low(m_debugDisplay.x, (uint)_mode); }
    DisplayMode     getDisplayMode          ()                                  { return (DisplayMode)unpackUint16low(m_debugDisplay.x); }
    
    void            setDisplayFlags         (DisplayFlags _flags)               { m_debugDisplay.x = packUint16high(m_debugDisplay.x, (uint)_flags); }
    DisplayFlags    getDisplayFlags         ()                                  { return (DisplayFlags)unpackUint16high(m_debugDisplay.x); }
    
    void            setToolmodeRWBufferID   (uint _id)                          { m_debugDisplay.z = (m_debugDisplay.z & ~0x0000FFFF) | _id; }
    uint            getToolmodeRWBufferID   ()                                  { return m_debugDisplay.z & 0x0000FFFF; }

    // Camera constants
    void            setCameraNearFar        (float2 _nearFar)                   { m_camera.xy = _nearFar; }
    void            setCameraFieldOfView    (float _fov)                        { m_camera.z = _fov; }
    void            setCameraAspectRatio    (float _ar)                         { m_camera.w = _ar; }

    float2          getCameraNearFar        ()                                  { return m_camera.xy; }
    float           getCameraDepthRange     ()                                  { return m_camera.y - 0 * m_camera.x; }
    float           getCameraFieldOfView    ()                                  { return m_camera.z; }
    float           getCameraAspectRatio    ()                                  { return m_camera.w; }
    
    void            setView                 (float4x4 _view)                    { m_view = _view; }
    float4x4        getView                 ()                                  { return m_view; }

    void            setViewInv              (float4x4 _viewInv)                 { m_viewInv = _viewInv; }
    float4x4        getViewInv              ()                                  { return m_viewInv; }
    
    void            setProj                 (float4x4 _proj)                    { m_proj = _proj; }
    float4x4        getProj                 ()                                  { return m_proj; }

    void            setProjInv              (float4x4 _projInv)                 { m_projInv = _projInv; }
    float4x4        getProjInv              ()                                  { return m_projInv; }
    
    float           getLinearDepth          (float _zBuffer)                    { float n = getCameraNearFar().x; float f = getCameraNearFar().y; return (n*f) / (f - _zBuffer * (f - n)); /*return (2.0f * n) / (f + n - _zBuffer * (f - n));*/ }
    float3          getViewPos              (float2 _screenPos, float _zBuffer) { float4 clipPos = float4(float2(_screenPos.x, 1-_screenPos.y) * 2.0f - 1.0f, _zBuffer, 1.0f); float4 viewPos = mul(clipPos, getProjInv()); return viewPos.xyz / viewPos.w; }
    float3          getWorldPos             (float2 _screenPos, float _zBuffer) { float3 viewPos = getViewPos(_screenPos, _zBuffer); return mul(float4(viewPos.xyz,1.0f), getViewInv()).xyz; }
    float3          getCameraRight          ()                                  {  return -m_viewInv[0].xyz; }
    float3          getCameraUp             ()                                  { return -m_viewInv[1].xyz; }
    float3          getCameraForward        ()                                  { return -m_viewInv[2].xyz; }
    float3          getCameraPos            ()                                  { return m_viewInv[3].xyz; }

    // Raytracing constants
    void            setTLASHandle           (uint _value)                       { m_rayTracing.x = (m_rayTracing.x & ~0x0000FFFFUL) | (_value & 0xFFFF); }
    uint            getTLASHandle           ()                                  { return 0xFFFF & m_rayTracing.x; }

    // Environment constants
    void            setEnvironmentColor     (float3 _environmentColor)          { m_environmentColor.rgb = _environmentColor; } 
    float3          getEnvironmentColor     ()                                  { return m_environmentColor.rgb; }

    void            setEnvironmentCubemap   (uint _texCubeHandle)               { m_textures.x = packUint16low(m_textures.x, _texCubeHandle); }
    uint            getEnvironmentCubemap   ()                                  { return unpackUint16low(m_textures.x); }

    void            setSpecularBRDF         (uint _tex2DHandle)                 { m_textures.x = packUint16high(m_textures.x, _tex2DHandle); }
    uint            getSpecularBRDF         ()                                  { return unpackUint16high(m_textures.x); }

    void            setIrradianceCubemap    (uint _texCubeHandle)               { m_textures.y = packUint16low(m_textures.y, _texCubeHandle); }
    uint            getIrradianceCubemap    ()                                  { return unpackUint16low(m_textures.y); }

    void            setSpecularReflectionCubemap(uint _texCubeHandle)           { m_textures.y = packUint16high(m_textures.y, _texCubeHandle); }
    uint            getSpecularReflectionCubemap()                              { return unpackUint16high(m_textures.y); }

    void            setIrradianceIntensity  (float _intensity)                  { m_pbr.x = _intensity; }
    float           getIrradianceIntensity  ()                                  { return m_pbr.x; }

    void            setSpecularReflectionIntensity(float _intensity)            { m_pbr.y = _intensity; }
    float           getSpecularReflectionIntensity()                            { return m_pbr.y; }

    // Lens constants
    void            setFocalLength          (float _focalLength)                { m_lens.x = _focalLength; }
    float           getFocalLength          ()                                  { return m_lens.x; }

    void            setAperture             (float _aperture)                   { m_lens.y = _aperture; }
    float           getAperture             ()                                  { return m_lens.y; }

    void            setFocusDistance        (float _focusDistance)              { m_lens.z = _focusDistance; }
    float           getFocusDistance        ()                                  { return m_lens.z; }

    void            setDOFScale             (float _dofScale)                   { m_lens.w = _dofScale; }
    float           getDOFScale             ()                                  { return m_lens.w; }
      
    float4          getZPassOutlineColor    (uint _index)                       { return m_outlineCategory[_index].zPassOutlineColor; }
    float4          getZFailOutlineColor    (uint _index)                       { return m_outlineCategory[_index].zFailOutlineColor; }
    float4          getZFailFillColor       (uint _index)                       { return m_outlineCategory[_index].zFailFillColor; }
    
    void setOutlineColors(uint _index, float4 _zPassOutline, float4 _zFailOutline, float4 _zFailFill) 
    { 
        m_outlineCategory[_index].zPassOutlineColor = _zPassOutline; 
        m_outlineCategory[_index].zFailOutlineColor = _zFailOutline;
        m_outlineCategory[_index].zFailFillColor = _zFailFill;
    }   
};

#endif // _VIEW__HLSLI_