#pragma once

#include "core/Types/Types.h"
#include "core/IResource.h"

namespace vg::core::io
{
    class Buffer;
}

namespace vg::renderer
{
    template <core::uint N> struct SkinVertex
    {
        core::u8 bone_index[N];
        core::u8 bone_weight[N];
    };

    class AnimNodeData
    {
    public:
        bool read(core::io::Buffer & _buffer);
        bool write(core::io::Buffer & _buffer) const;

        core::string                    name;
        float                           time_begin;
        float                           framerate;
        core::uint                      num_frames;
        core::quaternion                const_rot;
        core::float3                    const_pos;
        core::float3                    const_scale;
        core::vector<core::quaternion>  rot;
        core::vector<core::float3>      pos;
        core::vector<core::float3>      scale;
    };;

    class AnimImporterData
    {
    public:
        core::LoadStatus    load(const core::string & _file);
        bool                save(const core::string & _file) const;

        core::string                name;
        float                       time_begin;
        float                       time_end;
        float                       framerate;
        uint                        num_frames;
        core::vector<AnimNodeData>  animNodes;
    };
}