#include "glslang/StandAlone/DirStackFileIncluder.h"

namespace vg::graphics::driver::vulkan
{
    //--------------------------------------------------------------------------------------
    ShaderCompiler::ShaderCompiler()
    {
        glslang::InitializeProcess();
        initBuiltInResource();
    }

    //--------------------------------------------------------------------------------------
    ShaderCompiler::~ShaderCompiler()
    {
        glslang::FinalizeProcess();
    }

    //--------------------------------------------------------------------------------------
    EShLanguage getVulkanTargetProfile(ShaderStage _stage)
    {
        switch (_stage)
        {
            default:
                VG_ASSERT(false);

            case ShaderStage::Vertex:
                return EShLangVertex;

            case ShaderStage::Hull:
                return EShLangTessControl;

            case ShaderStage::Domain:
                return EShLangTessEvaluation;

            case ShaderStage::Geometry:
                return EShLangGeometry;

            case ShaderStage::Pixel:
                return EShLangFragment;

            case ShaderStage::Compute:
                return EShLangCompute;
        }
    }

    //--------------------------------------------------------------------------------------
    driver::Shader * ShaderCompiler::compile(const core::string & _path, const core::string & _entryPoint, ShaderStage _stage, vector<pair<string, uint>> & _macros)
    {
        string source;
        if (file::read(_path, source))
        {
            EShLanguage targetProfile = getVulkanTargetProfile(_stage);
            glslang::TShader tShader(targetProfile);

            // Add #define
            //_macros.push_back({ "VULKAN", 1 }); // 'VULKAN' macro is already defined

            string macros = "";
            for (auto & macro : _macros)
                macros = macros + "#define " + macro.first + " " + to_string(macro.second) + "\n";
            source = macros + source;

            const char * src = source.c_str();
            tShader.setStrings(&src, 1);

            const uint inputSemanticsVer = 110;                                                 // #define VULKAN 110
            const glslang::EShTargetClientVersion clientVersion = glslang::EShTargetVulkan_1_2; // Vulkan 1.2
            const glslang::EShTargetLanguageVersion targetVersion = glslang::EShTargetSpv_1_5;  // SPIR-V 1.5

            tShader.setEnvInput(glslang::EShSourceHlsl, targetProfile, glslang::EShClientVulkan, inputSemanticsVer);
            tShader.setEnvClient(glslang::EShClientVulkan, clientVersion);
            tShader.setEnvTarget(glslang::EShTargetSpv, targetVersion);
            tShader.setEntryPoint(_entryPoint.c_str());

            const EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgReadHlsl | EShMsgVulkanRules);

            DirStackFileIncluder includeHandler;
            std::string dir = file::getFileDir(_path);
            includeHandler.pushExternalLocalDirectory(dir);            

            std::string preprocessedSource;
            const int defaultVer = 100;
            const bool preprocessOK = tShader.preprocess(&m_builtInResource, defaultVer, ENoProfile, false, false, messages, &preprocessedSource, includeHandler);
            VG_ASSERT(preprocessOK, "Preprocess failed:\n%s\n%s", tShader.getInfoLog(), tShader.getInfoDebugLog());

            const char * preprocessedSrc = preprocessedSource.c_str();
            tShader.setStrings(&preprocessedSrc, 1);

            const bool parseOK = tShader.parse(&m_builtInResource, defaultVer, false, messages);
            VG_ASSERT(parseOK, "Parse failed:\n%s\n%s", tShader.getInfoLog(), tShader.getInfoDebugLog());

            glslang::TProgram tProgram;
            tProgram.addShader(&tShader);

            const bool linkOK = tProgram.link(messages);
            VG_ASSERT(parseOK, "Link failed:\n%s\n%s", tShader.getInfoLog(), tShader.getInfoDebugLog());

            std::vector<u32> spv;
            spv::SpvBuildLogger spvLogger;
            glslang::SpvOptions spvOptions;
            glslang::GlslangToSpv(*tProgram.getIntermediate(targetProfile), spv, &spvLogger, &spvOptions);

            if (spvLogger.getAllMessages().length() > 0)
            {
                VG_DEBUGPRINT("%s\n", spvLogger.getAllMessages());
            }

            core::Blob blob(spv.data(), (uint)spv.size());
            auto * shader = new driver::Shader(blob);
            return shader;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void ShaderCompiler::initBuiltInResource()
    {
        m_builtInResource.maxLights = 32;
        m_builtInResource.maxClipPlanes = 6;
        m_builtInResource.maxTextureUnits = 32;
        m_builtInResource.maxTextureCoords = 32;
        m_builtInResource.maxVertexAttribs = 64;
        m_builtInResource.maxVertexUniformComponents = 4096;
        m_builtInResource.maxVaryingFloats = 64;
        m_builtInResource.maxVertexTextureImageUnits = 32;
        m_builtInResource.maxCombinedTextureImageUnits = 80;
        m_builtInResource.maxTextureImageUnits = 32;
        m_builtInResource.maxFragmentUniformComponents = 4096;
        m_builtInResource.maxDrawBuffers = 32;
        m_builtInResource.maxVertexUniformVectors = 128;
        m_builtInResource.maxVaryingVectors = 8;
        m_builtInResource.maxFragmentUniformVectors = 16;
        m_builtInResource.maxVertexOutputVectors = 16;
        m_builtInResource.maxFragmentInputVectors = 15;
        m_builtInResource.minProgramTexelOffset = -8;
        m_builtInResource.maxProgramTexelOffset = 7;
        m_builtInResource.maxClipDistances = 8;
        m_builtInResource.maxComputeWorkGroupCountX = 65535;
        m_builtInResource.maxComputeWorkGroupCountY = 65535;
        m_builtInResource.maxComputeWorkGroupCountZ = 65535;
        m_builtInResource.maxComputeWorkGroupSizeX = 1024;
        m_builtInResource.maxComputeWorkGroupSizeY = 1024;
        m_builtInResource.maxComputeWorkGroupSizeZ = 64;
        m_builtInResource.maxComputeUniformComponents = 1024;
        m_builtInResource.maxComputeTextureImageUnits = 16;
        m_builtInResource.maxComputeImageUniforms = 8;
        m_builtInResource.maxComputeAtomicCounters = 8;
        m_builtInResource.maxComputeAtomicCounterBuffers = 1;
        m_builtInResource.maxVaryingComponents = 60;
        m_builtInResource.maxVertexOutputComponents = 64;
        m_builtInResource.maxGeometryInputComponents = 64;
        m_builtInResource.maxGeometryOutputComponents = 128;
        m_builtInResource.maxFragmentInputComponents = 128;
        m_builtInResource.maxImageUnits = 8;
        m_builtInResource.maxCombinedImageUnitsAndFragmentOutputs = 8;
        m_builtInResource.maxCombinedShaderOutputResources = 8;
        m_builtInResource.maxImageSamples = 0;
        m_builtInResource.maxVertexImageUniforms = 0;
        m_builtInResource.maxTessControlImageUniforms = 0;
        m_builtInResource.maxTessEvaluationImageUniforms = 0;
        m_builtInResource.maxGeometryImageUniforms = 0;
        m_builtInResource.maxFragmentImageUniforms = 8;
        m_builtInResource.maxCombinedImageUniforms = 8;
        m_builtInResource.maxGeometryTextureImageUnits = 16;
        m_builtInResource.maxGeometryOutputVertices = 256;
        m_builtInResource.maxGeometryTotalOutputComponents = 1024;
        m_builtInResource.maxGeometryUniformComponents = 1024;
        m_builtInResource.maxGeometryVaryingComponents = 64;
        m_builtInResource.maxTessControlInputComponents = 128;
        m_builtInResource.maxTessControlOutputComponents = 128;
        m_builtInResource.maxTessControlTextureImageUnits = 16;
        m_builtInResource.maxTessControlUniformComponents = 1024;
        m_builtInResource.maxTessControlTotalOutputComponents = 4096;
        m_builtInResource.maxTessEvaluationInputComponents = 128;
        m_builtInResource.maxTessEvaluationOutputComponents = 128;
        m_builtInResource.maxTessEvaluationTextureImageUnits = 16;
        m_builtInResource.maxTessEvaluationUniformComponents = 1024;
        m_builtInResource.maxTessPatchComponents = 120;
        m_builtInResource.maxPatchVertices = 32;
        m_builtInResource.maxTessGenLevel = 64;
        m_builtInResource.maxViewports = 16;
        m_builtInResource.maxVertexAtomicCounters = 0;
        m_builtInResource.maxTessControlAtomicCounters = 0;
        m_builtInResource.maxTessEvaluationAtomicCounters = 0;
        m_builtInResource.maxGeometryAtomicCounters = 0;
        m_builtInResource.maxFragmentAtomicCounters = 8;
        m_builtInResource.maxCombinedAtomicCounters = 8;
        m_builtInResource.maxAtomicCounterBindings = 1;
        m_builtInResource.maxVertexAtomicCounterBuffers = 0;
        m_builtInResource.maxTessControlAtomicCounterBuffers = 0;
        m_builtInResource.maxTessEvaluationAtomicCounterBuffers = 0;
        m_builtInResource.maxGeometryAtomicCounterBuffers = 0;
        m_builtInResource.maxFragmentAtomicCounterBuffers = 1;
        m_builtInResource.maxCombinedAtomicCounterBuffers = 1;
        m_builtInResource.maxAtomicCounterBufferSize = 16384;
        m_builtInResource.maxTransformFeedbackBuffers = 4;
        m_builtInResource.maxTransformFeedbackInterleavedComponents = 64;
        m_builtInResource.maxCullDistances = 8;
        m_builtInResource.maxCombinedClipAndCullDistances = 8;
        m_builtInResource.maxSamples = 4;
        m_builtInResource.maxMeshOutputVerticesNV = 256;
        m_builtInResource.maxMeshOutputPrimitivesNV = 512;
        m_builtInResource.maxMeshWorkGroupSizeX_NV = 32;
        m_builtInResource.maxMeshWorkGroupSizeY_NV = 1;
        m_builtInResource.maxMeshWorkGroupSizeZ_NV = 1;
        m_builtInResource.maxTaskWorkGroupSizeX_NV = 32;
        m_builtInResource.maxTaskWorkGroupSizeY_NV = 1;
        m_builtInResource.maxTaskWorkGroupSizeZ_NV = 1;
        m_builtInResource.maxMeshViewCountNV = 4;

        m_builtInResource.limits.nonInductiveForLoops = 1;
        m_builtInResource.limits.whileLoops = 1;
        m_builtInResource.limits.doWhileLoops = 1;
        m_builtInResource.limits.generalUniformIndexing = 1;
        m_builtInResource.limits.generalAttributeMatrixVectorIndexing = 1;
        m_builtInResource.limits.generalVaryingIndexing = 1;
        m_builtInResource.limits.generalSamplerIndexing = 1;
        m_builtInResource.limits.generalVariableIndexing = 1;
        m_builtInResource.limits.generalConstantMatrixVectorIndexing = 1;
    }
}