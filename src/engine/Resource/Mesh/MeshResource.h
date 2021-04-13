#pragma once

#include "core/Resource/Resource.h"

namespace vg::graphics::renderer
{
    class IMeshModel;
}

namespace vg::engine
{
    class MeshResource : public core::Resource
    {
    public:
        MeshResource(IObject * _owner);
        ~MeshResource();

        const core::vector<core::string>    getExtensions   () const final;
        void                                onPathChanged   (IObject * _owner, const core::string & _oldPath, const core::string & _newPath) final;
        bool                                loadResource    (const core::string & _path, core::IObject * _owner) final override;

        graphics::renderer::IMeshModel *    getMeshModel() const { return m_meshModel; }

    private:
        graphics::renderer::IMeshModel *    m_meshModel = nullptr;
    };
}