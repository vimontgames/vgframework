#pragma once

#include "FrameGraph_consts.h"
#include "gfx/CommandList/CommandList_consts.h"
#include "ResourceTransition.h"
#include "gfx/Resource/Texture_consts.h"
#include "gfx/Device/Device_consts.h"

namespace vg::gfx
{
    class Texture;
    class Buffer;
    class UserPass;

    vg_enum_class(FrameGraphResourceType, core::u8,
        Buffer = 0,
        Texture
    );

    class FrameGraphResource
    {
    public:

        enum class InitState : core::u8
        {
            Discard = 0x00,
            Clear = 0x01,
            Preserve = 0x02
        };

        struct PassRWAccess
        {
            PassRWAccess(const UserPass * _userPass, RWFlags _rwAccess) :
                m_userPass(_userPass),
                m_rwAccess(_rwAccess)
            {

            }

            const UserPass * m_userPass;
            RWFlags				m_rwAccess;
        };

        FrameGraphResource();

        virtual ~FrameGraphResource()
        {
            m_readWrite.clear();
        }

        void setType(FrameGraphResourceType _type);
        FrameGraphResourceType getType() const;

        void setName(const core::string & _name);
        const core::string & getName() const;

        void setReadAtPass(const UserPass * _subPass);
        void setWriteAtPass(const UserPass * _subPass);
        void setReadWriteAtPass(const UserPass * _subPass);

        bool isRead(const UserPass * _userPass) const;
        bool isFirstRead(const UserPass * _userPass) const;
        bool isLastRead(const UserPass * _userPass) const;

        bool isWrite(const UserPass * _userPass) const;
        bool isFirstWrite(const UserPass * _userPass) const;
        bool isLastWrite(const UserPass * _userPass) const;
        bool isReadAfter(const UserPass * _userPass) const;

        bool needsReadToWriteTransition(const UserPass * _userPass) const;
        bool needsWriteToReadTransition(const UserPass * _userPass) const;

        const core::vector<PassRWAccess> & getReadWriteAccess() const;

        void setCurrentState(ResourceState _state);
        ResourceState getCurrentState() const;

    private:
        FrameGraphResourceType          m_type;
        core::string                    m_name;
        core::vector<PassRWAccess>		m_readWrite;
        ResourceState                   m_state = ResourceState::Undefined;
    };

    struct FrameGraphTextureResourceDesc 
    {
        FrameGraphTextureResourceDesc()
        {

        }

        FrameGraphTextureResourceDesc(const FrameGraphTextureResourceDesc & _other) :
            type(_other.type),
            width(_other.width),
            height(_other.height),
            depth(_other.depth),
            format(_other.format),
            msaa(_other.msaa),
            initState(_other.initState),
            clearColor(_other.clearColor),
            transient(_other.transient),
            renderTarget(_other.renderTarget),
            uav(_other.uav)
        {

        }

        FrameGraphTextureResourceDesc & operator=(const FrameGraphTextureResourceDesc & _other)
        {
            type = _other.type;
            width = _other.width;
            height = _other.height;
            depth = _other.depth;
            format = _other.format;
            msaa = _other.msaa;
            initState = _other.initState;
            clearColor = _other.clearColor;
            transient = _other.transient;
            renderTarget = _other.renderTarget;
            uav = _other.uav;

            return *this;
        }

        TextureType                     type        = TextureType::Texture2D;
        core::u16			            width       = 1;
        core::u16			            height      = 1;
        core::u16                       depth       = 1;
        PixelFormat			            format      = PixelFormat::R8G8B8A8_unorm;
        MSAA                            msaa        = MSAA::None;
        FrameGraphResource::InitState   initState   = FrameGraphResource::InitState::Discard;
        union
        {
            core::float4		        clearColor = defaultOptimizedClearColor;
            struct
            {
                float                   clearDepth;
                core::u8                clearStencil;
            };
        };

        bool    transient = false;
        bool    renderTarget = false;
        bool    uav = false;

        bool operator == (const FrameGraphTextureResourceDesc & _other) const
        {
            return transient == _other.transient
                && uav == _other.uav
                && renderTarget == _other.renderTarget
                && width == _other.width
                && height == _other.height
                && format == _other.format          // TODO: alias textures of the same size but different format (store format, init state, clear color elsewhere ?)
                && msaa == _other.msaa
                && initState == _other.initState
                && hlslpp::all(clearColor == _other.clearColor);
        }
    };

    struct FrameGraphBufferResourceDesc
    {
        FrameGraphBufferResourceDesc()
        {

        }

        FrameGraphBufferResourceDesc(const FrameGraphBufferResourceDesc & _other) :
            elementSize(_other.elementSize),
            elementCount(_other.elementCount),
            transient(_other.transient),
            uav(_other.uav)
        {

        }

        core::u32 elementSize = 0;
        core::u32 elementCount = 1;
        bool    transient = false;
        bool    uav = false;

        bool operator == (const FrameGraphBufferResourceDesc & _other) const
        {
            return transient == _other.transient
                   && uav == _other.uav
                   && elementSize == _other.elementSize
                   && elementCount == _other.elementCount;
        }
    };

    class FrameGraphTextureResource : public FrameGraphResource
    {
    public:
        void                                    setTextureResourceDesc(const FrameGraphTextureResourceDesc & _texResDesc);
        const FrameGraphTextureResourceDesc &   getTextureResourceDesc() const;
        void                                    setTexture(Texture * _tex);
        Texture *                               getTexture() const;
        void                                    resetTexture();

    private:
        FrameGraphTextureResourceDesc		    m_desc;
        Texture *                               m_texture = nullptr;
    };

    struct FrameGraphBufferResource : public FrameGraphResource
    {
    public:
        enum class Usage : core::u16
        {
            None = 0x0000
        };

        void                                    setBufferResourceDesc(const FrameGraphBufferResourceDesc & _bufResDesc, Buffer * _buffer = nullptr);
        const FrameGraphBufferResourceDesc &    getBufferResourceDesc() const;
        void                                    setBuffer(Buffer * _buffer);
        Buffer *                                getBuffer() const;
        void                                    resetBuffer();

    private:
        FrameGraphBufferResourceDesc            m_desc;
        Buffer *                                m_buffer = nullptr;
    };

    struct FrameGraphResourceTransitionDesc
    {
        FrameGraphResource *    m_resource;
        ResourceTransitionDesc  m_transitionDesc;
    };
}