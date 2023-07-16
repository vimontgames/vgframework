#pragma once

#include "core/Object/Object.h"
#include "FrameGraph_consts.h"
#include "graphics/driver/IView.h"

namespace vg::graphics::driver
{
	class Texture;
    class TextureDesc;
	class Buffer;
    class BufferDesc;
	class UserPass;
	class RenderPass;

	enum class PixelFormat : core::u8;

    struct RenderContext
    {
        IView * m_view;

		static const core::string MakeUniqueName(const core::string & _name, ViewID _viewID)
		{
			return _name + (core::string)"-" + core::to_string(_viewID);

		}

		const core::string getName(const core::string & _name) const
		{
			return MakeUniqueName(_name, m_view->GetViewID());
		}
    };

	class FrameGraph : public core::Object
	{
	public:
        struct UserPassInfo
        {
            RenderContext	m_renderContext;
            UserPass *		m_userPass;
        };

        const char * getClassName() const final { return "FrameGraph"; }

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

			const core::vector<const UserPass *> & getReadAtPass() const;
			const core::vector<const UserPass *> & getWriteAtPass() const;

            void setCurrentState(ResourceState _state);
            ResourceState getCurrentState() const;

		private:
			Type                            m_type;	
            core::string                    m_name;
			core::vector<const UserPass*>   m_read;
			core::vector<const UserPass*>	m_write;
            ResourceState                   m_state = ResourceState::Undefined;
		};

		struct TextureResourceDesc
		{
            TextureResourceDesc()
            {

            }

            TextureResourceDesc(const TextureResourceDesc & _from) :
                width(_from.width),
                height(_from.height),
                format(_from.format),
                initState(_from.initState),
                clearColor(_from.clearColor),
                transient(_from.transient)
            {

            }

			core::u16			width		= 0;
			core::u16			height		= 0;
			PixelFormat			format		= (PixelFormat)0;
			Resource::InitState initState	= Resource::InitState::Discard;
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

            bool operator == (const TextureResourceDesc & _other) const
            {
                return  width == _other.width
                    && height == _other.height
                    && format == _other.format          // TODO: alias textures of the same size but different format (store format, initstate, clear color elsewhere ?)
                    && initState == _other.initState    
                    && hlslpp::all(clearColor == _other.clearColor)  
                    && transient == _other.transient;   
            }
		};

		struct BufferResourceDesc
		{
			core::u32 size = 0;
		};

		class TextureResource : public Resource
		{
		public:
			void                        setTextureResourceDesc  (const TextureResourceDesc & _texResDesc);
            const TextureResourceDesc & getTextureResourceDesc  () const;
            void                        setTexture              (Texture * _tex);
			Texture *                   getTexture              () const;
            void                        resetTexture            ();

		private:
			TextureResourceDesc		    m_desc;
			Texture *	                m_texture = nullptr;
		};

		struct BufferResource : public Resource
		{
		public:
			enum class Usage : core::u16
			{
				None = 0x0000
			};

			void                        setBufferResourceDesc   (const BufferResourceDesc & _bufResDesc, Buffer * _buffer = nullptr);
            const BufferResourceDesc &  getBufferResourceDesc   () const;
            Buffer *                    getBuffer               () const;

		private:
			BufferResourceDesc		    m_desc;
			Buffer *	                m_buffer = nullptr;
		};

		FrameGraph();
		~FrameGraph();

        void destroyTransientResources();

		void importRenderTarget(const ResourceID & _resID, Texture * _tex, core::float4 _clearColor = core::float4(0, 0, 0, 0), FrameGraph::Resource::InitState _initState = FrameGraph::Resource::InitState::Clear);
		void setGraphOutput(const ResourceID & _destTexResID);

		void setup(double _dt);
		void build();
		void render();

        bool addUserPass(const RenderContext & _renderContext, UserPass * _userPass, const UserPassID & _renderPassID);

        template <class T> T * getResource(Resource::Type _type, const ResourceID & _resID, bool _mustExist);

        TextureResource *   getTextureResource  (const ResourceID & _resID) const;
        BufferResource *    getBufferResource   (const ResourceID & _resID) const;

        TextureResource *   addTextureResource  (const ResourceID & _resID, const TextureResourceDesc & _texResDesc, Texture * _tex = nullptr);
        BufferResource *    addBufferResource   (const ResourceID & _resID, const BufferResourceDesc & _bufResDesc, Buffer * _buf = nullptr);

		void setResized(); 

	private:

        Texture * createRenderTargetFromPool(const TextureResourceDesc & _textureResourceDesc);
        Texture * createDepthStencilFromPool(const TextureResourceDesc & _textureResourceDesc);

        Texture * createTextureFromPool(const FrameGraph::TextureResourceDesc & _textureResourceDesc, bool _depthStencil);

        void releaseTextureFromPool(Texture *& _tex);

		void findDependencies(const UserPass & _renderPassDesc, core::uint _depth);
		void reverseAndRemoveDuplicates();
		void allocateResources();

        void cleanup();

	private:
        using resource_unordered_map = core::unordered_map<FrameGraph::ResourceID, Resource*, core::hash<ResourceID>>;
        resource_unordered_map		m_resources;

		core::vector<UserPassInfo>	m_userPassInfo;
		core::vector<RenderPass*>	m_renderPasses;

        struct SharedTexture
        {
            TextureResourceDesc desc;
            Texture * tex = nullptr;
            bool used = false;
        };
        core::vector<SharedTexture>     m_sharedTextures;

		ResourceID                      m_outputResID;
        TextureResource *               m_outputRes = nullptr;
		bool							m_resized = false;
	};
}