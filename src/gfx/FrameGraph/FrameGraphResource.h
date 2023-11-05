#pragma once

#include "FrameGraph_consts.h"

namespace vg::gfx
{
    class Texture;
    class Buffer;
    class UserPass;

    enum class PixelFormat : core::u8;

    class FrameGraphResource
    {
    public:
        enum class Type : core::u8
        {
            Buffer = 0,
            Texture
        };

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

        void setType(Type _type);
        Type getType() const;

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

        bool needsReadToWriteTransition(const UserPass * _userPass) const;
        bool needsWriteToReadTransition(const UserPass * _userPass) const;

        const core::vector<PassRWAccess> & getReadWriteAccess() const;

        void setCurrentState(ResourceState _state);
        ResourceState getCurrentState() const;

    private:
        Type                            m_type;
        core::string                    m_name;
        core::vector<PassRWAccess>		m_readWrite;
        ResourceState                   m_state = ResourceState::Undefined;
    };

    struct FrameGraphTextureResourceDesc
    {
        FrameGraphTextureResourceDesc()
        {

        }

        FrameGraphTextureResourceDesc(const FrameGraphTextureResourceDesc & _from) :
            width(_from.width),
            height(_from.height),
            format(_from.format),
            initState(_from.initState),
            clearColor(_from.clearColor),
            transient(_from.transient),
            uav(_from.uav)
        {

        }

        core::u16			width = 0;
        core::u16			height = 0;
        PixelFormat			format = (PixelFormat)0;
        FrameGraphResource::InitState initState = FrameGraphResource::InitState::Discard;
        union
        {
            core::float4		clearColor = (core::float4)0.0f;
            struct
            {
                float           clearDepth;
                core::u8        clearStencil;
            };
        };
        bool                transient = false;
        bool				uav = false;

        bool operator == (const FrameGraphTextureResourceDesc & _other) const
        {
            return  width == _other.width
                && height == _other.height
                && format == _other.format          // TODO: alias textures of the same size but different format (store format, init state, clear color elsewhere ?)
                && initState == _other.initState
                && hlslpp::all(clearColor == _other.clearColor)
                && transient == _other.transient
                && uav == _other.uav;
        }
    };

    struct FrameGraphBufferResourceDesc
    {
        core::u32 size = 0;
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
        Buffer *                                getBuffer() const;

    private:
        FrameGraphBufferResourceDesc            m_desc;
        Buffer *                                m_buffer = nullptr;
    };
}