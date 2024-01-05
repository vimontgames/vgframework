#pragma once

#include "displaymodes.hlsli"

//--------------------------------------------------------------------------------------
// This file is only included in HLSL shader files. 
//--------------------------------------------------------------------------------------

float3 getMatIDColor(uint _matID)
{
    switch (_matID % 6)
    {
        case 0:
            return float3(1, 0, 0);
        case 1:
            return float3(0, 1, 0);
        case 2:
            return float3(1, 1, 0);
        case 3:
            return float3(0, 0, 1);
        case 4:
            return float3(1, 0, 1);
        case 5:
            return float3(0, 1, 1);
    }

    return 0;
}

float4 forwardDebugDisplay(float4 _color, DisplayMode _mode, uint _matID, float3 _tan, float3 _bin, float3 _nrm, float4 _col, float2 _uv0, float2 _uv1, float2 _screenPos, float3 _worldPos, float3 _albedo, float3 _normal)
{
    switch (_mode)
    {
        default:
        case DisplayMode::None:
            return _color;
    
        case DisplayMode::Forward_MatID:
            return sRGBA2Linear(float4(getMatIDColor(_matID), 1.0f));
            
        case DisplayMode::Forward_VSTangent:
            return sRGBA2Linear(float4(normalize(_tan) * 0.5f + 0.5f, 1.0f));
        
        case DisplayMode::Forward_VSBinormal:
            return sRGBA2Linear(float4(normalize(_bin) * 0.5f + 0.5f, 1.0f));

        case DisplayMode::Forward_VSNormal:
            return sRGBA2Linear(float4(normalize(_nrm) * 0.5f + 0.5f, 1.0f));
        
        case DisplayMode::Forward_VSColor:
            return _col;
        
        case DisplayMode::Forward_UV0:
            return sRGBA2Linear(float4(_uv0.xy, 0, 1));
        
        case DisplayMode::Forward_UV1:
            return sRGBA2Linear(float4(_uv1.xy, 0, 1));
        
        case DisplayMode::Forward_Albedo:
            return float4(_albedo.rgb, 1);
        
        case DisplayMode::Forward_PSNormal:
            return sRGBA2Linear(float4(_normal.xyz * 0.5 + 0.5, 1));

        case DisplayMode::Forward_WorldPos:
            return sRGBA2Linear(float4(frac(_worldPos.xyz), 1));
        
        case DisplayMode::Forward_ScreenPos:
            return sRGBA2Linear(float4(_screenPos.xy, 0, 1));

        case DisplayMode::Forward_SurfaceType:
            #if _ALPHABLEND
            return sRGBA2Linear(float4(1,0,0, 1));
            #elif _ALPHATEST
            return sRGBA2Linear(float4(1,1,0, 0));
            #else
            return sRGBA2Linear(float4(0,1,0, 1));
            #endif
    }
}