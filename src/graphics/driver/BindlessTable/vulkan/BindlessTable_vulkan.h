#pragma once

namespace vg::graphics::driver::vulkan
{
    class BindlessTable : public base::BindlessTable
    {
        using super = base::BindlessTable;

    public:
        BindlessTable();
        ~BindlessTable();

        void copyTextureHandle(core::uint _slot, driver::Texture * _texture);

    private:

    };
}