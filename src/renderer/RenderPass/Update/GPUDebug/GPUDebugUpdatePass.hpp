#include "GPUDebugUpdatePass.h"
#include "Shaders/system/debug.hlsli"
#include "core/File/File.h"

namespace vg::renderer
{
    static const uint s_GPUDebugBufferSize = GPUDEBUG_TOTAL_BUFFER_SIZE;

    //--------------------------------------------------------------------------------------
    GPUDebugUpdatePass::GPUDebugUpdatePass() :
        UpdatePass("GPUDebugUpdatePass")
    {
        auto * device = Device::get();

        BufferDesc GPUDebugBufferDesc = BufferDesc(Usage::Default, BindFlags::UnorderedAccess, CPUAccessFlags::Write, BufferFlags::None, s_GPUDebugBufferSize);
        m_GPUDebugBuffer = device->createBuffer(GPUDebugBufferDesc, "GPUDebug", nullptr, ReservedSlot::GPUDebugBufRW);
    }

    //--------------------------------------------------------------------------------------
    GPUDebugUpdatePass::~GPUDebugUpdatePass()
    {
        VG_SAFE_RELEASE(m_GPUDebugBuffer);
        VG_SAFE_RELEASE(m_GPUDebugBufferStaging);
    }

    //--------------------------------------------------------------------------------------
    void GPUDebugUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderContext, gfx::CommandList * _cmdList)
    {
        if (m_GPUDebugBufferStaging != nullptr)
        {
            u32 * data = (u32*)m_GPUDebugBufferStaging->getResource().map().data;
            {
                const uint requested = data[0];
                VG_ASSERT(requested < (uint)GPUDEBUG_TOTAL_BUFFER_SIZE, "[GPUDebug] The GPUDebugBuffer is %u bytes but trying to write %u bytes.\n Please increase buffer size or reduce GPU debug output size.", GPUDEBUG_TOTAL_BUFFER_SIZE, requested);
                const uint totalSize = min(data[0], (uint)GPUDEBUG_TOTAL_BUFFER_SIZE);

                if (totalSize > 0)
                {
                    const string rootFolder = core::io::getRootDirectory() + "/";

                    u8 * p = (u8 *)&data[1];

                    while ((p - (u8*)data) < totalSize)
                    {
                        char temp[256];
                        uint index = 0;

                        uint header1 = (*(uint *)p);
                        const GPUDebugMsgType msgType = (GPUDebugMsgType)(header1 & 0xFF);
                        const uint msgArgCount = (header1 >> 8) & 0xFF;
                        const uint stringSize = header1 >> 16;
                        p += 4;

                        uint header2 = (*(uint *)p);
                        uint line = header2 & 0xFFFF;
                        uint fileID = header2 >> 16;
                        p += 4;

                        const auto shaderManager = ShaderManager::get();
                        const string * pfilename = shaderManager->getShaderFilePathFromID(fileID);
                        string formattedFilename = pfilename ? *pfilename : "";
                        formattedFilename = rootFolder + formattedFilename;

                        u8 * c = p;
                        while (*c != 0)
                        {
                            temp[index] = *c;
                            ++index;
                            ++c;
                        }

                        p = p + stringSize;

                        if (index > 0)
                        {
                            temp[index] = 0;

                            // count args in string
                            uint argCount = 0;
                            for (uint i = 0; i < index; ++i)
                            {
                                if (temp[i] == '%')
                                    argCount++;
                            }

                            VG_ASSERT(msgArgCount == argCount, "String format \"%s\" expects %u args, but %u were provided in file %s(%u) ", temp, argCount, msgArgCount, formattedFilename.c_str(), line);
                            argCount = min(argCount, msgArgCount);

                            string tempString = temp;
                            auto argPos = tempString.find('%');

                            u32 * args = (u32*)core::alignUp((uint_ptr)p, (uint_ptr)4);

                            for (uint i = 0; i < argCount; ++i)
                            {
                                size_t endArg = tempString.length();
                            
                                bool started = false;
                                for (auto j = argPos; j < tempString.length(); ++j)
                                {
                                    char c = tempString[j];
                                    if ((c >= '0' && c <= '9') || c == '.' || c == '%' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'A'))
                                    {
                                        if (c == '%')
                                        {
                                            if (started)
                                            {
                                                endArg = j;
                                                break;
                                            }
                                            else
                                            {
                                                started = true;
                                            }
                                        }
                                        continue;
                                    }
                                    else
                                    {
                                        endArg = j;
                                        break;
                                    }
                                }                               
                            
                                string arg = tempString.substr(argPos, endArg-argPos);
                                char argType = arg[arg.length() - 1];
                                switch (argType)
                                {
                                    default:
                                        VG_ASSERT("[GPUDebug] Unknow arg type \"%s\" used. Only \"%u\" and \"%%f\" args are supported", arg.c_str());
                                        break;
                            
                                    case 'u':
                                        tempString.replace(argPos, arg.length(), fmt::sprintf(arg, args[i]));
                                        break;

                                    case 'i':
                                        tempString.replace(argPos, arg.length(), fmt::sprintf(arg, (int &)args[i]));
                                        break;
                            
                                    case 'f':
                                        tempString.replace(argPos, arg.length(), fmt::sprintf(arg, (float &)args[i]));
                                        break;
                                }
                            
                                argPos = tempString.find('%', argPos + 1);
                            }
                            

                            p = (u8 *)args;

                            // skip args
                            for (uint a = 0; a < msgArgCount; ++a)
                            {
                                uint value = *(uint *)p;
                                p += 4;
                            }                           

                            switch (msgType)
                            {
                                default:
                                    VG_DEBUGPRINT("[GPUDebug] Unknown message type 0x%08X\n", msgType);
                                    break;

                                case GPUDebugMsgType::None:
                                    VG_DEBUGPRINT("%s(%u): %s\n", formattedFilename.c_str(), line, tempString.c_str()); // Unlike VG_INFO/VG_WARNING/VG_ERROR, VG_DEBUGPRINT does not automatically add EOL
                                break;

                                case GPUDebugMsgType::Info:
                                    VG_INFO("%s(%u): %s", formattedFilename.c_str(), line, tempString.c_str());
                                    break;

                                case GPUDebugMsgType::Warning:
                                    VG_WARNING("%s(%u): %s", formattedFilename.c_str(), line, tempString.c_str());
                                    break;

                                case GPUDebugMsgType::Error:
                                    VG_ERROR("%s(%u): %s", formattedFilename.c_str(), line, tempString.c_str());
                                    break;

                                case GPUDebugMsgType::Assert:
                                {
                                    static bool skip = false;								
                                    if (vg::core::assertmsg("[GPUAssert]", __func__, pfilename? pfilename->c_str() : "", line, skip, "%s", tempString.c_str()))
                                        VG_DEBUG_BREAK();
                                }
                                break;
                            } 
                        }
                        else
                        {
                            ++p;
                        }
                    };
                }
            }
            m_GPUDebugBufferStaging->getResource().unmap();
        }
        else
        {
            // allocate staging copy
            BufferDesc stagingDesc = m_GPUDebugBuffer->getBufDesc();
            stagingDesc.resource.m_usage = Usage::Staging;
            stagingDesc.resource.m_bindFlags = BindFlags::None;
            m_GPUDebugBufferStaging = gfx::Device::get()->createBuffer(stagingDesc, "GPUDebugStaging");
        }

        // copy to staging
        _cmdList->copyBuffer(m_GPUDebugBufferStaging, m_GPUDebugBuffer, 0);

        // Clear
        _cmdList->clearRWBuffer(m_GPUDebugBuffer, 0x0);
    }
}
