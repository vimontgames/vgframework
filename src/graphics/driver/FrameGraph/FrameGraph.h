#pragma once

#include "core/Object/Object.h"
#include "FrameGraph_consts.h"

namespace vg::graphics::driver
{
	class Texture;
	class Buffer;
	class UserPass;
	class RenderPass;

	enum class PixelFormat : core::u8;

	class FrameGraph : public core::Object
	{
	public:
		using RenderPassID = core::string;
		using ResourceID = core::string;

		class Resource : public core::Object
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

			void setReadAtPass(const UserPass * _subPass);
			void setWriteAtPass(const UserPass * _subPass);

			const core::unordered_set<const UserPass *> & getReadAtPass() const;
			const core::unordered_set<const UserPass *> & getWriteAtPass() const;

		private:
			Type								m_type;	
			core::unordered_set<const UserPass*>	m_read;
			core::unordered_set<const UserPass*>	m_write;
		};

		struct TextureDesc
		{
			core::u16			width		= 0;
			core::u16			height		= 0;
			PixelFormat			format		= (PixelFormat)0;
			Resource::InitState initState	= Resource::InitState::Discard;
			core::float4		clearColor	= core::float4(0,0,0,0);
		};

		struct BufferDesc
		{
			core::u32 size = 0;
		};

		class TextureResource : public Resource
		{
		public:

			enum class Usage : core::u16
			{
				None		 = 0x0000,
				Present		 = 0x0001,
				RenderTarget = 0x0002
			};

			void setTextureDesc(const TextureDesc & _resDesc);
			void setTextureUsage(Usage _usage);
			void setTexture(const Texture * _texture);

			const TextureDesc & getTextureDesc() const;
			Usage getTextureUsage() const;
			const Texture * getTexture() const;

		private:
			TextureDesc		m_desc;
			Usage			m_usage;
			const Texture *	m_texture = nullptr;
		};

		struct BufferResource : public Resource
		{
		public:
			enum class Usage : core::u16
			{
				None = 0x0000
			};

			void setBufferDesc(const BufferDesc & _resDesc);
			void setBufferUsage(Usage _usage);
			void setBuffer(const Buffer * _buffer);

		private:
			BufferDesc		m_desc;
			Usage			m_usage;
			const Buffer *	m_buffer = nullptr;
		};

		FrameGraph();
		~FrameGraph();

		void import(const ResourceID & _resID, Texture * _tex);
		void setGraphOutput(const ResourceID & _destTexResID);

		void setup();
		void build();
		void render();

		template <class T> T & addUserPass(const RenderPassID & _renderPassID)
		{
			auto it = m_subPasses.find(_renderPassID);
			if (m_subPasses.end() == it)
			{
				T * newPass = new T();
				m_subPasses[_renderPassID] = newPass;

				newPass->setName(_renderPassID);
				newPass->setFrameGraph(this);

				return *newPass;
			}
			else
				return static_cast<T&>(*it->second);
		}

		template <class T> T & addResource(Resource::Type _type, const ResourceID & _resID)
		{
			auto it = m_resources.find(_resID);
			if (m_resources.end() == it)
			{
				T * newResource = new T();
				m_resources[_resID] = newResource;

				newResource->setType(_type);
				newResource->setName(_resID);

				return *newResource;
			}
			else
			{
				VG_ASSERT(_type == it->second->getType());
				return static_cast<T&>(*it->second);
			}
		}

		TextureResource & addTextureResource(const ResourceID & _resID)
		{
			return addResource<TextureResource>(Resource::Type::Texture, _resID);
		}

		BufferResource & addBufferResource(const ResourceID & _resID)
		{
			return addResource<BufferResource>(Resource::Type::Buffer, _resID);
		}

	private:
		Resource & addResource(Resource::Type _type, const ResourceID & _resID);

		void findDependencies(const UserPass & _renderPassDesc, core::uint _depth);
		void reverseAndRemoveDuplicates();
		void allocateResources();

        void cleanup();

	private:
		core::unordered_map<FrameGraph::RenderPassID, UserPass*>	m_subPasses;
		core::unordered_map<FrameGraph::ResourceID, Resource*>	m_resources;

		core::vector<const UserPass*>							m_userPassStack;
		core::vector<RenderPass*>								m_renderPasses;

		ResourceID												m_outputResID;
	};
}