#ifndef _VERTEX__HLSLI_
#define _VERTEX__HLSLI_

#include "types.hlsli"
#include "packing.hlsli"
#include "view.hlsli"

#define GPU_VERTEXBUFFER_OFFSET_ALIGNMENT 4

// List every vertex format here (max 255 because it's serialized as u8 and 0xFF stands for "invalid vertex format")
vg_enum_class_global(VertexFormat, uint,
    Default         = 0,
    Skinning_4Bones = 1,
    DebugDrawUnlit  = 2,
    DebugDrawLit    = 3,
    ParticleQuad    = 4
);

//--------------------------------------------------------------------------------------
// TODO: move to HLSL math header?
//--------------------------------------------------------------------------------------
inline float4x4 getMatrixWithoutScale(float4x4 _matrix)
{
    float4x4 matrix = _matrix;
    matrix[0].xyz = matrix[0].xyz / length(_matrix[0].xyz);
    matrix[1].xyz = matrix[1].xyz / length(_matrix[1].xyz);
    matrix[2].xyz = matrix[2].xyz / length(_matrix[2].xyz);
    return matrix;
}

inline constexpr uint getVertexFormatStride(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::DebugDrawUnlit:
            return 16;

        case VertexFormat::DebugDrawLit:
            return 28;
        
        default:
        case VertexFormat::Default:
            return 68;
            
        case VertexFormat::Skinning_4Bones:
            return 80;
        
        case VertexFormat::ParticleQuad:
            return 28;
    }
}

inline bool hasNormal(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
        case VertexFormat::DebugDrawLit:
            return true;
        
        default:
        case VertexFormat::DebugDrawUnlit:
            return false;
    }
}

inline bool hasBinormal(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
            return true;
        
        default:
        case VertexFormat::DebugDrawLit:
        case VertexFormat::DebugDrawUnlit:
            return false;
    }
}

inline bool hasTangent(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
            return true;
        
        default:
        case VertexFormat::DebugDrawLit:
        case VertexFormat::DebugDrawUnlit:
            return false;
    }
}

inline bool hasUV0(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
            return true;
        
        default:
        case VertexFormat::DebugDrawLit:
        case VertexFormat::DebugDrawUnlit:
            return false;
    }
}

inline bool hasUV1(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
            return true;
        
        default:
        case VertexFormat::DebugDrawLit:
        case VertexFormat::DebugDrawUnlit:
            return false;
    }
}

inline bool hasColor(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Default:
        case VertexFormat::Skinning_4Bones:
        case VertexFormat::DebugDrawLit:
        case VertexFormat::DebugDrawUnlit:
            return true;
        
        default:
            return false;
    }
}

inline bool hasSkinning(VertexFormat _format)
{
    switch (_format)
    {
        case VertexFormat::Skinning_4Bones:
            return true;
        
        default:
            return false;
    }
}

struct Vertex
{    
#ifndef __cplusplus
    void Clear()
    {
        pos = float3(0, 0, 0);
        nrm = float3(0, 0, 1);
        bin = float3(0, 1, 0);
        tan = float3(1, 0, 0);
        uv[0] = float2(0, 0);
        uv[1] = float2(1, 1);
        color = 0xFFFFFFFF;
    }
    
    #if _PARTICLE
    void Load(ByteAddressBuffer _buffer, VertexFormat _format, uint _vertexID, uint _offset, float4x4 _viewInv)
    {       
        uint particleVertexID = _vertexID / 4;
        uint cornerIndex = _vertexID % 4;
    
        uint offset = _offset + particleVertexID * getVertexFormatStride(_format);   
    
        Clear(); 
    
        pos.xyz = _buffer.Load<float3>(offset); 
        offset += 3 * sizeof(float);
    
        float frame = _buffer.Load<float>(offset); 
        offset += sizeof(float); 
    
        float2 size = _buffer.Load<float2>(offset); 
        offset += 2 * sizeof(float); 
    
         color = _buffer.Load<uint>(offset);
        offset += sizeof(uint);
    
        // Particle billboard position
        float2 quadOffsets[4] =
        {
            float2(-1, -1),
            float2(-1, +1),
            float2(+1, -1),
            float2(+1, +1)
        };
    
        float2 quadOffset = quadOffsets[cornerIndex] * size;
        
        float3 up    = _viewInv[1].xyz;
        float3 right = _viewInv[0].xyz;
        float3 forward = _viewInv[2].xyz;
    
        pos = pos + right * quadOffset.x + up * quadOffset.y;
    
        // Particle billboard tangent-space
        nrm = forward;
        bin = right;
        tan = up;
    
        float2 quadUV[4] = 
        {
            float2(0,1),
            float2(0,0),
            float2(1,1),
            float2(1,0)
        };
        uv[0] = quadUV[cornerIndex];
    }
    #else
    
    void Load(ByteAddressBuffer _buffer, VertexFormat _format, uint _vertexID, uint _offset = 0, float4x4 _unusedViewInv = (float4x4) 0.0f)
    {
        uint offset = _offset + _vertexID * getVertexFormatStride(_format);    

        Clear();    

        pos.xyz = _buffer.Load<float3>(offset); 
        offset += 3 * sizeof(float);
        
        if (hasNormal(_format))
        {
            nrm.xyz = _buffer.Load<float3>(offset);
            offset += 3 * sizeof(float);
        }
        
        if (hasBinormal(_format))
        {
            bin.xyz = _buffer.Load<float3>(offset);
            offset += 3 * sizeof(float);
        }
        
        if (hasTangent(_format))
        {
            tan.xyz = _buffer.Load<float3>(offset);
            offset += 3 * sizeof(float);
        }
        
        if (hasUV0(_format))
        {
            uv[0].xy= _buffer.Load<float2>(offset);
            offset += 2 * sizeof(float);
        }
        
        if (hasUV1(_format))
        {
            uv[1].xy    = _buffer.Load<float2>(offset);
            offset += 2 * sizeof(float);
        }
        
        if (hasColor(_format))
        {
            color = _buffer.Load<uint>(offset);
            offset += sizeof(uint);
        }
        
        if (hasSkinning(_format))
        {
            uint2 temp0123 = _buffer.Load<uint2>(offset);
            uint2 temp01 = unpackUint16(temp0123.x);
            skinIndices[0] = temp01.x;
            skinIndices[1] = temp01.y;
            uint2 temp23 = unpackUint16(temp0123.y);
            skinIndices[2] = temp23.x;
            skinIndices[3] = temp23.y;
            offset += 8;   
            
            float4 tempW = unpackRGBA8(_buffer.Load<uint>(offset));
            skinWeights[0] = tempW.r;
            skinWeights[1] = tempW.g;
            skinWeights[2] = tempW.b;
            skinWeights[3] = tempW.a;
            offset += 4;
        }
    }
    #endif
    
    void Store(RWByteAddressBuffer _rwbuffer, VertexFormat _format, uint _vertexID, uint _offset = 0)
    {
        uint offset = _offset + _vertexID * getVertexFormatStride(_format);
        
        _rwbuffer.Store<float3>(offset, pos.xyz); 
        offset += 3 * sizeof(float);
        
        if (hasNormal(_format))
        {
            _rwbuffer.Store<float3>(offset, nrm.xyz);
            offset += 3 * sizeof(float);
        }
        
        if (hasBinormal(_format))
        {
            _rwbuffer.Store<float3>(offset, bin.xyz);
            offset += 3 * sizeof(float);
        }
        
        if (hasTangent(_format))
        {
            _rwbuffer.Store<float3>(offset, tan.xyz);
            offset += 3 * sizeof(float);
        }
        
        if (hasUV0(_format))
        {
            _rwbuffer.Store<float2>(offset, uv[0].xy);
            offset += 2 * sizeof(float);
        }
        
        if (hasUV1(_format))
        {
            _rwbuffer.Store<float2>(offset, uv[1].xy);
            offset += 2 * sizeof(float);
        }
        
        if (hasColor(_format))
        {
            _rwbuffer.Store<uint>(offset, color);
            offset += 4;
        }
    }

    template<typename T> T Interpolate(T _v0, T _v1, T _v2, float3 _bary)    {  return _v0 * _bary.x + _v1 * _bary.y + _v2 * _bary.z; }

    void Interpolate(Vertex verts[3], float3 _bary)
    {
        Clear();

        pos = Interpolate(verts[0].getPos(), verts[1].getPos(), verts[2].getPos(), _bary);
        nrm = Interpolate(verts[0].getNrm(), verts[1].getNrm(), verts[2].getNrm(), _bary);
        bin = Interpolate(verts[0].getBin(), verts[1].getBin(), verts[2].getBin(), _bary);
        tan = Interpolate(verts[0].getTan(), verts[1].getTan(), verts[2].getTan(), _bary);
        uv[0] = Interpolate(verts[0].getUV(0), verts[1].getUV(0), verts[2].getUV(0), _bary);
        uv[1] = Interpolate(verts[0].getUV(1), verts[1].getUV(1), verts[2].getUV(1), _bary);
        color = packRGBA8(Interpolate(verts[0].getColor(), verts[1].getColor(), verts[2].getColor(), _bary));
    }
    #endif

    float3 getPos()             { return pos; }
    float3 getNrm()             { return nrm; }
    float3 getBin()             { return bin; }
    float3 getTan()             { return tan; }
    float2 getUV(uint _index)   { return uv[_index]; }
    float4 getColor()           { return unpackRGBA8(color); }

    float3 pos;
    float3 nrm;
    float3 bin;
    float3 tan;
    float2 uv[2];
    uint color;
    uint skinIndices[4];
    float skinWeights[4];
};

#endif // _VERTEX__HLSLI_