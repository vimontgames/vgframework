#pragma once

#include "ShaderKey.h"

namespace vg::gfx
{
    enum class API : core::u8;
    class Shader;

    struct GraphicsPrograms
    {
        Shader * m_shaders[core::enumCount<GraphicsStage>()] = {};
        void release();
    };

    struct ComputePrograms
    {
        Shader * m_shaders[core::enumCount<ComputeStage>()] = {};
        void release();
    };

    struct RayTracingPrograms
    {
        Shader * m_shaders[core::enumCount<RayTracingStage>()] = {};
        void release();
    };

    class HLSLDesc
    {
    public:
        struct Technique
        {
            HLSLDesc *          desc = nullptr;
            core::string        name;
            ShaderKey::Flags    flags = 0x0000;

            void setFlags(ShaderKey::Flags _flags)
            {
                flags = _flags;
            }
        };

        struct GraphicsTechnique : public Technique
        {
            ShaderKey::VS           vs = (ShaderKey::VS)-1;
            ShaderKey::HS           hs = (ShaderKey::HS)-1;
            ShaderKey::DS           ds = (ShaderKey::DS)-1;
            ShaderKey::GS           gs = (ShaderKey::GS)-1;
            ShaderKey::PS           ps = (ShaderKey::PS)-1;   

            void setVertexShader(const core::string & _entryPoint)
            {
                vs = (ShaderKey::VS)desc->addGraphicsEntryPoint(GraphicsStage::Vertex, _entryPoint);
            }

            void setHullShader(const core::string & _entryPoint)
            {
                hs = (ShaderKey::HS)desc->addGraphicsEntryPoint(GraphicsStage::Hull, _entryPoint);
            }

            void setDomainShader(const core::string & _entryPoint)
            {
                ds = (ShaderKey::DS)desc->addGraphicsEntryPoint(GraphicsStage::Domain, _entryPoint);
            }

            void setGeometryShader(const core::string & _entryPoint)
            {
                gs = (ShaderKey::GS)desc->addGraphicsEntryPoint(GraphicsStage::Geometry, _entryPoint);
            }

            void setPixelShader(const core::string & _entryPoint)
            {
                ps = (ShaderKey::VS)desc->addGraphicsEntryPoint(GraphicsStage::Pixel, _entryPoint);
            }
        };

        struct ComputeTechnique : public Technique
        {
            ComputeShaderKey::CS    cs = (ComputeShaderKey::CS)-1;

            void setComputeShader(const core::string & _entryPoint)
            {
                cs = (ComputeShaderKey::CS)desc->addComputeEntryPoint(ComputeStage::Compute, _entryPoint);
            }
        };

        struct RayTracingTechnique : public Technique
        {
            RayTracingShaderKey::RG rg = (RayTracingShaderKey::RG)-1;
            RayTracingShaderKey::AH ah = (RayTracingShaderKey::AH)-1;
            RayTracingShaderKey::CH ch = (RayTracingShaderKey::CH)-1;
            RayTracingShaderKey::MI mi = (RayTracingShaderKey::MI)-1;
            RayTracingShaderKey::IS is = (RayTracingShaderKey::IS)-1;

            void setRayGenShader(const core::string & _entryPoint)
            {
                rg = (RayTracingShaderKey::RG)desc->addRayTracingEntryPoint(RayTracingStage::RayGen, _entryPoint);
            }

            void setAnyHitShader(const core::string & _entryPoint)
            {
                ah = (RayTracingShaderKey::AH)desc->addRayTracingEntryPoint(RayTracingStage::AnyHit, _entryPoint);
            }

            void setClosestHitShader(const core::string & _entryPoint)
            {
                ch = (RayTracingShaderKey::CH)desc->addRayTracingEntryPoint(RayTracingStage::ClosestHit, _entryPoint);
            }

            void setMissShader(const core::string & _entryPoint)
            {
                mi = (RayTracingShaderKey::MI)desc->addRayTracingEntryPoint(RayTracingStage::Miss, _entryPoint);
            }

            void setIntersectionShader(const core::string & _entryPoint)
            {
                is = (RayTracingShaderKey::AH)desc->addRayTracingEntryPoint(RayTracingStage::Intersection, _entryPoint);
            }
        };

        ~HLSLDesc(); // release all compiled shaders

        void                                                setFile                     (const core::string & _file);
        const core::string &                                getFile                     () const;

        ShaderKey::Flags                                    declareFlag                 (core::uint _index, const core::string & _define);

        GraphicsTechnique &                                 addGraphicsTechnique        (const core::string & _name);
        const core::vector<GraphicsTechnique> &             getGraphicsTechniques       () const;

        ComputeTechnique &                                  addComputeTechnique         (const core::string & _name);
        const core::vector<ComputeTechnique> &              getComputeTechniques        () const;

        RayTracingTechnique &                               addRayTracingTechnique      (const core::string & _name);
        const core::vector<RayTracingTechnique> &           getRayTracingTechniques     () const;

        core::u64                                           getCRC                      () const { return m_crc; }
        void                                                setCRC                      (core::u64 _crc) { m_crc = _crc; }

        void                                                release                     ();

        // Graphics
        ShaderKey::EntryPoint                               addGraphicsEntryPoint       (GraphicsStage _stage, const core::string & _entryPoint);
        const core::string &                                getGraphicsEntryPoint       (GraphicsStage _stage, ShaderKey::EntryPoint _index) const;
        bool                                                getGraphicsPrograms         (API _api, const ShaderKey & _key, GraphicsPrograms & _programs);

        // Compute
        ComputeShaderKey::EntryPoint                        addComputeEntryPoint        (ComputeStage _stage, const core::string & _entryPoint);
        const core::string &                                getComputeEntryPoint        (ComputeStage _stage, ComputeShaderKey::EntryPoint _index) const;
        bool                                                getComputePrograms          (API _api, const ComputeShaderKey & _key, ComputePrograms & _programs);

        // RayTracing
        RayTracingShaderKey::EntryPoint                     addRayTracingEntryPoint     (RayTracingStage _stage, const core::string & _entryPoint);
        const core::string &                                getRayTracingEntryPoint     (RayTracingStage _stage, RayTracingShaderKey::EntryPoint _index) const;
        bool                                                getRayTracingPrograms       (API _api, const RayTracingShaderKey & _key, RayTracingPrograms & _programs);

    protected:
        
        core::vector<core::pair<core::string, core::uint>>  getShaderMacros             (ShaderKey::Flags _flags) const;

    private:

        struct ShaderFlagDesc
        {
            inline ShaderFlagDesc(const core::string & _define = {}) :
                m_define(_define)
            {

            }

            inline void clear()
            {
                m_define = {};
            }

            core::string        m_define;
        };

        core::u64                           m_crc = 0;
        core::string                        m_file;
        ShaderFlagDesc                      m_flagDescs[sizeof(ShaderKey::Flags)<<3];

        core::vector<core::string>          m_graphicsEntryPoint[core::enumCount<GraphicsStage>()];
        core::vector<core::string>          m_computeEntryPoint[core::enumCount<ComputeStage>()];
        core::vector<core::string>          m_rayTracingEntryPoint[core::enumCount<RayTracingStage>()];

        core::vector<GraphicsTechnique>     m_graphicsTechniques;
        core::vector<ComputeTechnique>      m_computeTechniques;
        core::vector<RayTracingTechnique>   m_rayTracingTechniques;

        using GraphicsVariantCollection = core::map<ShaderKey, GraphicsPrograms>;
        GraphicsVariantCollection           m_graphicsVariants;

        using ComputeVariantCollection = core::map<ComputeShaderKey, ComputePrograms>;
        ComputeVariantCollection            m_computeVariants;

        using RayTracingVariantCollection = core::map<RayTracingShaderKey, RayTracingPrograms>;
        ComputeVariantCollection            m_rayTracingVariants;
    };
}