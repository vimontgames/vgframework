#pragma once

#include "imguiAdapter.h"

namespace vg::renderer::dx12
{
    class ImGuiAdapter : public vg::renderer::base::ImGuiAdapter
    {
        protected:
            void        init                    ();
            void        deinit                  ();

            void        beginFrame              ();
            void        render                  (gfx::CommandList * _cmdList);

            ImTextureID addTexture              (const gfx::Texture * _texture);
            void        removeTexture           (ImTextureID id);

            void        updateBackbufferFormat  ();

        private:
            void        updateDescriptors       ();

    };
}