#pragma once

namespace vg::gfx::vulkan
{
    class BindlessTable : public base::BindlessTable
    {
        using super = base::BindlessTable;

    public:
        BindlessTable();
        ~BindlessTable();

        void copyTextureHandle(core::uint _slot, gfx::Texture * _texture);

    private:

    };
}