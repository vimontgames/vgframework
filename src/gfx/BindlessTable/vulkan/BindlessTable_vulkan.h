#pragma once

namespace vg::gfx::vulkan
{
    class BindlessTable : public base::BindlessTable
    {
        using super = base::BindlessTable;

    public:
        BindlessTable();
        ~BindlessTable();

        void    updateBindlessTLASHandle    (const BindlessTLASHandle & _handle, const gfx::TLAS * _tlas);
        void    copyTextureHandle           (core::uint _slot, gfx::Texture * _texture);
    };
}