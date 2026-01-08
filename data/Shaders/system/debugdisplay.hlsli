#ifndef _DEBUG_DISPLAY__HLSLI_
#define _DEBUG_DISPLAY__HLSLI_

#include "displaymodes.hlsli"
#include "material_consts.hlsli"

//--------------------------------------------------------------------------------------
// This file is only included in HLSL shader files. 
//--------------------------------------------------------------------------------------
float4 getWireframeColor(GPUInstanceType _gpuInstanceType, GPUInstanceFlags _gpuInstanceFlags)
{   
    float4 color;
    bool isStatic = HasAnyFlag(_gpuInstanceFlags, GPUInstanceFlags::Static);
    
    switch (_gpuInstanceType)
    {
        case GPUInstanceType::Mesh:
            color = isStatic ? float4(0.5f, 1.0f, 0.5f, 1) : float4(0, 1, 0, 1);
            break;
        
        case GPUInstanceType::ParticleSystem:
            color = float4(1, 0, 0, 1);
            break;
        
        default:
            color =  float4(1, 1, 1, 1);
            break;
    }
            
    return color;
}

float3 getMatIDColor(uint _matID)
{       
    float grey = 1.0 - pow((_matID - _matID % 6) / 300.0f, 0.25);
    
    switch (_matID % 6)
    {
        case 0:
            return grey * float3(1, 0, 0);
        case 1:
            return grey * float3(0, 1, 0);
        case 2:
            return grey * float3(1, 1, 0);
        case 3:
            return grey * float3(0, 0, 1);
        case 4:
            return grey * float3(1, 0, 1);
        case 5:
            return grey * float3(0, 1, 1);
    }

    return 0;
}

float3 getSurfaceTypeColor(SurfaceType _surfaceType)
{
    switch (_surfaceType)
    {
        case SurfaceType::Opaque:
            return float3(0, 1, 0);
        
        case SurfaceType::AlphaTest:
            return float3(1, 1, 0);

        case SurfaceType::AlphaBlend:
            return float3(1, 0, 0);

        case SurfaceType::Decal:
            return float3(0, 0, 1);
    }
    
    return float3(0, 0, 0);
}

float4 forwardDebugDisplay(float4 _color, GPUInstanceData _instanceData, DisplayMode _mode, uint _matID, float3 _tan, float3 _bin, float3 _nrm, float4 _col, float2 _uv0, float2 _uv1, float2 _screenPos, float3 _worldPos, float3 _albedo, float3 _normal, float3 _worldNormal, float3 _pbr)
{
    switch (_mode)
    {
        default:
        case DisplayMode::None:
            return _color;
    
        case DisplayMode::Geometry_MaterialID:
            return SRGBAToLinear(float4(getMatIDColor(_matID), 1.0f));
            
        case DisplayMode::Geometry_VertexTangent:
            return SRGBAToLinear(float4(normalize(_tan) * 0.5f + 0.5f, 1.0f));
        
        case DisplayMode::Geometry_VertexBinormal:
            return SRGBAToLinear(float4(normalize(_bin) * 0.5f + 0.5f, 1.0f));

        case DisplayMode::Geometry_VertexNormal:
            return SRGBAToLinear(float4(normalize(_nrm) * 0.5f + 0.5f, 1.0f));
        
        case DisplayMode::Geometry_VertexColor:
            return _col;
        
        case DisplayMode::Geometry_UV0:
            return SRGBAToLinear(float4(frac(_uv0.xy), any(saturate(_uv0) != _uv0) ? 1 : 0, 1));
        
        case DisplayMode::Geometry_UV1:
            return SRGBAToLinear(float4(frac(_uv1.xy), any(saturate(_uv0) != _uv0) ? 1 : 0, 1));
        
        case DisplayMode::Instance_Static:
        {
            GPUInstanceFlags flags = _instanceData.getGPUInstanceFlags();
            //GPUInstanceFlags instanceType = (GPUInstanceFlags) ((uint) flags & GPUInstanceFlags_TypeMask);
            //
            //switch (instanceType)
            //{
            //    default:
            //        _color.rgb = float3(1, 1, 1);
            //        break;
            //    
            //    case GPUInstanceFlags::Mesh:
            //        _color.rgb = float3(0, 1, 0);
            //        break;
            //    
            //    case GPUInstanceFlags::Particle:
            //        _color.rgb = float3(1, 0, 0);
            //        break;
            //}
            
            if (HasAnyFlag(flags, GPUInstanceFlags::Static))
                _color.rgb = lerp(_color.rgb, float3(0, 1, 0), 0.5f);
            else
                _color.rgb = lerp(_color.rgb, float3(1, 0, 0), 0.5f);

            return _color;
        };
        
        // TODO: use GPUInstanceData/GPUMaterialData directly here instead?
        case DisplayMode::Instance_Color:
        case DisplayMode::Material_Albedo:
            return float4(_albedo.rgb, 1);

        case DisplayMode::Material_AmbientOcclusion:
            return float4(_pbr.rrr, 1);

        case DisplayMode::Material_Roughness:
            return float4(_pbr.ggg, 1);

        case DisplayMode::Material_Metalness:
            return float4(_pbr.bbb, 1);
        
        case DisplayMode::Material_NormalMap:
            return SRGBAToLinear(float4(_normal.xyz * 0.5 + 0.5, 1));

        case DisplayMode::Forward_WorldNormal:
            return SRGBAToLinear(float4(_worldNormal.xyz * 0.5 + 0.5, 1));

        case DisplayMode::Forward_WorldPosition:
            return SRGBAToLinear(float4(frac(_worldPos.xyz), 1));
        
        case DisplayMode::Forward_ScreenPos:
            return SRGBAToLinear(float4(_screenPos.xy, 0, 1));

        case DisplayMode::Forward_SurfaceType:
            #if _ALPHABLEND
            return SRGBAToLinear(float4(getSurfaceTypeColor(SurfaceType::AlphaBlend), 1));
            #elif _ALPHATEST
            return SRGBAToLinear(float4(getSurfaceTypeColor(SurfaceType::AlphaTest), 1));
            #elif _DECAL
            return SRGBAToLinear(float4(getSurfaceTypeColor(SurfaceType::Decal), 1));
            #else
            return SRGBAToLinear(float4(getSurfaceTypeColor(SurfaceType::Opaque), 1));
            #endif
    }
}

#endif