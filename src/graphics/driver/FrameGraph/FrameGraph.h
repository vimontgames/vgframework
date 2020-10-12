#pragma once

#include "core/Object/Object.h"
#include "FrameGraph_consts.h"

namespace vg::graphics::driver
{
	class Texture;
    class TextureDesc;
	class Buffer;
    class BufferDesc;
	class UserPass;
	class RenderPass;

	enum class PixelFormat : core::u8;

	class FrameGraph : public core::Object
	{
	public:
		using UserPassID = core::string;
		using ResourceID = core::string;

		class Resource
		{
		public:
			enum class Type : core::u8
			{
				Buffer = 0,
				Texture
			};

			enum class InitState : core::u8
			{
				Discard		= 0x00,
				Clear		= 0x01,
				Preserve	= 0x02
			};

			Resource();

			virtual ~Resource()
			{
				m_read.clear();
				m_write.clear();
			}

			void setType(Type _type);
			Type getType() const;

            void setName(const core::string & _name);
            const core::string & getName() const;

			void setReadAtPass(const UserPass * _subPass);
			void setWriteAtPass(const UserPass * _subPass);

			const core::unordered_set<const UserPass *> & getReadAtPass() const;
			const core::unordered_set<const UserPass *> & getWriteAtPass() const;

		private:
			Type								    m_type;	
            core::string                            m_name;
			core::unordered_set<const UserPass*>	m_read;
			core::unordered_set<const UserPass*>	m_write;
		};

		struct TextureResourceDesc
		{
			core::u16			width		= 0;
			core::u16			height		= 0;
			PixelFormat			format		= (PixelFormat)0;
			Resource::InitState initState	= Resource::InitState::Discard;
			core::float4		clearColor	= core::float4(0,0,0,0);
		};

		struct BufferResourceDesc
		{
			core::u32 size = 0;
		};

		class TextureResource : public Resource
		{
		public:
			void                        setTextureResourceDesc  (const TextureResourceDesc & _texResDesc, const Texture * _tex);
            const TextureResourceDesc & getTextureResourceDesc  () const;
			const Texture *             getTexture              () const;

		private:
			TextureResourceDesc		    m_desc;
			const Texture *	            m_texture = nullptr;
		};

		struct BufferResource : public Resource
		{
		public:
			enum class Usage : core::u16
			{
				None = 0x0000
			};

			void                        setBufferResourceDesc   (const BufferResourceDesc & _bufResDesc, const Buffer * _buffer);
            const BufferResourceDesc &  getBufferResourceDesc   () const;
            const Buffer *              getBuffer               () const;

		private:
			BufferResourceDesc		    m_desc;
			const Buffer *	            m_buffer = nullptr;
		};

		FrameGraph();
		~FrameGraph();

		void importRenderTarget(const ResourceID & _resID, Texture * _tex, core::float4 _clearColor = core::float4(0, 0, 0, 0), FrameGraph::Resource::InitState _initState = FrameGraph::Resource::InitState::Clear);
		void setGraphOutput(const ResourceID & _destTexResID);

		void setup();
		void build();
		void render();

        bool addUserPass(UserPass * _userPass, const UserPassID & _renderPassID);

        template <class T> T * getResource(Resource::Type _type, const ResourceID & _resID, bool _mustExist);

        TextureResource *   getTextureResource  (const ResourceID & _resID) const;
        BufferResource *    getBufferResource   (const ResourceID & _resID) const;

        TextureResource *   addTextureResource  (const ResourceID & _resID, const TextureResourceDesc & _texResDesc, const Texture * _tex = nullptr);
        BufferResource *    addBufferResource   (const ResourceID & _resID, const BufferResourceDesc & _bufResDesc, const Buffer * _buf = nullptr);

	private:

		void findDependencies(const UserPass & _renderPassDesc, core::uint _depth);
		void reverseAndRemoveDuplicates();
		void allocateResources();

        void cleanup();

	private:
        using userpass_unordered_map = core::unordered_map<FrameGraph::UserPassID, UserPass*, core::hash<FrameGraph::UserPassID>>;
        using resource_unordered_map = core::unordered_map<FrameGraph::ResourceID, Resource*, core::hash<FrameGraph::ResourceID>>;

        userpass_unordered_map          m_subPasses;
        resource_unordered_map          m_resources;

		core::vector<const UserPass*>   m_userPassStack;
		core::vector<RenderPass*>       m_renderPasses;

		ResourceID                      m_outputResID;
	};
}