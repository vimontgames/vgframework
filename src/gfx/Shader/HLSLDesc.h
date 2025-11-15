#pragma once

#include "Shader_consts.h"
#include "ShaderKey.h"
#include "ComputeShaderKey.h"

namespace vg::gfx
{
    enum class API : core::u8;
    class Shader;

    class HLSLDesc
    {
    public:
        enum Flags : ShaderKey::Flags
        {
            // Flags 10..15 are reserved and should be common for all shaders 
            Particle    = 10,   // Shader is used to render a quad particle (1 bit)
            Surface     = 11,   // SurfaceType (Opaque/AlphaTest/AlphaBlend/Decal) (2 bits)

            ZOnly       = 13,   // Depth-only shader (depth pre-pass or shadow pass) (1 bit)
            RayTracing  = 14,   // Shader is used with ray tracing enabled (1 bit)
            Toolmode    = 15    // Enable tool mode
        };

        struct Technique
        {
            core::string            name;

            ShaderKey::VS           vs = (ShaderKey::VS)-1;
            ShaderKey::HS           hs = (ShaderKey::HS)-1;
            ShaderKey::DS           ds = (ShaderKey::DS)-1;
            ShaderKey::GS           gs = (ShaderKey::GS)-1;
            ShaderKey::PS           ps = (ShaderKey::PS)-1;

            ComputeShaderKey::CS    cs = (ComputeShaderKey::CS)-1;

            ShaderKey::Flags        flags = 0x0000;

            VG_INLINE void setFlag(ShaderKey::Flags _flag, bool _enable = true);
            template <typename E> void setValue(ShaderKey::Flags _flag, E _value) { setFlags(_flag, (core::uint)_value); }
            VG_INLINE bool operator == (const Technique & _other) const;

        private:
            VG_INLINE void setFlags(ShaderKey::Flags _flag, core::uint _value);
        };

        ~HLSLDesc(); // release all compiled shaders

        void setFile(const core::string & _file);
        const core::string & getFile() const;

        void declFlag(core::uint _index, ShaderStageFlags _stages, const core::string & _define);
        void declFlags(core::uint _startIndex, ShaderStageFlags _stages, const core::vector<core::string> & _defines);

        core::uint getBitMask(core::uint _index) const
        {
            return m_flagDescs[_index].getBitMask();
        }

        VG_INLINE bool isValidFlagIndex(core::uint _index) const;
        bool isValidFlagValue(core::uint _index, core::uint _value) const;

        ShaderKey::VS declVS(const core::string & _vsEntryPoint);
        ShaderKey::HS declHS(const core::string & _hsEntryPoint);
        ShaderKey::DS declDS(const core::string & _dsEntryPoint);
        ShaderKey::GS declGS(const core::string & _gsEntryPoint);
        ShaderKey::PS declPS(const core::string & _psEntryPoint);

        ComputeShaderKey::CS declCS(const core::string & _csEntryPoint);

        Shader * getVS(API _api, ShaderKey::VS _vs, ShaderKey::Flags _flags);
        Shader * getHS(API _api, ShaderKey::HS _hs, ShaderKey::Flags _flags);
        Shader * getDS(API _api, ShaderKey::DS _ds, ShaderKey::Flags _flags);
        Shader * getGS(API _api, ShaderKey::GS _gs, ShaderKey::Flags _flags);
        Shader * getPS(API _api, ShaderKey::PS _ps, ShaderKey::Flags _flags);

        ShaderKey::EntryPoint declShader(ShaderStage _stage, const core::string & _entryPoint);
        Shader * getShader(API _api, ShaderStage _stage, ShaderKey::EntryPoint _index, ShaderKey::Flags _flags);

        Technique & declTechnique(const core::string & _name);
        Technique & declTechnique(const core::string & _name, ShaderKey::VS _vs, ShaderKey::PS _ps);
        const core::vector<Technique> & getTechniques() const;

        core::u64 getCRC() const;
        void setCRC(core::u64 _crc);

        void reset();
        void loadFromCache();

    protected:
        core::vector<core::pair<core::string, core::uint>> getShaderMacros(API _api, ShaderStage _stage, ShaderKey::Flags _flags) const;

    private:
        core::string getCookedShaderPath() const;

        struct VariantKey
        {
            explicit VariantKey() :
                m_bits(0x0)
            {

            }

            inline VariantKey(ShaderStage _stage, ShaderKey::EntryPoint _entryPoint, ShaderKey::Flags _flags) :
                m_stage(_stage),
                m_entryPoint(_entryPoint),
                m_flags(_flags)
            {

            }

            union
            {
                struct
                {
                    ShaderStage             m_stage;
                    ShaderKey::EntryPoint   m_entryPoint;
                    ShaderKey::Flags        m_flags;
                };
                core::u32 m_bits;
            };

            bool operator < (const VariantKey & other) const
            {
                return m_bits < other.m_bits;
            }
        };

        VariantKey computeVariantKey(ShaderStage _stage, ShaderKey::EntryPoint _index, ShaderKey::Flags _flags) const;

        using VariantCollection = core::map<VariantKey, Shader *>;

        struct CachedShaderHeader
        {
            core::u32               version;
            core::u64               crc;
            core::u32               size;
            HLSLDesc::VariantKey    key;
        };

        struct ShaderFlagDesc
        {
            inline ShaderFlagDesc()
            {

            }

            inline ShaderFlagDesc(ShaderStageFlags _stages, const core::string & _define) :
                m_stages(_stages)
            {
                m_defines.push_back("");
                m_defines.push_back(_define);
                m_bits = 1;
            }

            inline ShaderFlagDesc(ShaderStageFlags _stages, const core::vector<core::string> & _defines) :
                m_stages(_stages)
            {
                VG_ASSERT(_defines.size() > 0);
                m_defines = _defines;
                m_bits = computeBitCount((core::uint)_defines.size());
            }

            inline void clear()
            {
                m_stages = (ShaderStageFlags)0x0;
                m_defines.clear();
                m_bits = 0;
            }

            inline bool isInitialized() const
            {
                return (ShaderStageFlags)0x0 != m_stages && 0 != m_bits  && !m_defines.empty();
            }

            inline core::uint getBitMask() const
            {
                return (1 << m_bits) - 1;
            }

            inline static core::uint computeBitCount(core::uint _size)
            {
                return (core::uint)log2(_size - 1) + 1;
            }

            ShaderStageFlags            m_stages = (ShaderStageFlags)0x0;
            core::vector<core::string>  m_defines;
            core::uint                  m_bits = 0;
        };

        core::u64                       m_crc = 0;
        core::string                    m_file;
        ShaderFlagDesc                  m_flagDescs[sizeof(ShaderKey::Flags)<<3];
        core::vector<core::string>      m_entryPoint[core::enumCount<ShaderStage>()];
        core::vector<Technique>         m_techniques;
        VariantCollection               m_variants;
    };
}

#if VG_ENABLE_INLINE
#include "HLSLDesc.inl"
#endif