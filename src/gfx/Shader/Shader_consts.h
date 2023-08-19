#pragma once

//#define ENUMFLAGS(T)    template<class T> inline T   operator ~  (T a)          { return (T)~(std::underlying_type_t<T>)a; } \
//                        template<class T> inline T   operator |  (T a, T b)     { return (T)((std::underlying_type_t<T>)a | (std::underlying_type_t<T>)b); } \
//                        template<class T> inline T   operator &  (T a, T b)     { return (T)((std::underlying_type_t<T>)a & (std::underlying_type_t<T>)b); } \
//                        template<class T> inline T   operator ^  (T a, T b)     { return (T)((std::underlying_type_t<T>)a ^ (std::underlying_type_t<T>)b); } \
//                        template<class T> inline T & operator |= (T & a, T b)   { return (T &)((std::underlying_type_t<T> &)a |= (std::underlying_type_t<T>)b); } \
//                        template<class T> inline T & operator &= (T & a, T b)   { return (T &)((std::underlying_type_t<T> &)a &= (std::underlying_type_t<T>)b); } \
//                        template<class T> inline T & operator ^= (T & a, T b)   { return (T &)((std::underlying_type_t<T> &)a ^= (std::underlying_type_t<T>)b); }

namespace vg::gfx
{
    enum class ShaderStage : core::u8
    {
        Vertex = 0,
        Hull,
        Domain,
        Geometry,
        Pixel,

        Compute
    };

    enum class ShaderStageFlags : core::u16
    {
        VS = 1 << core::asInteger(ShaderStage::Vertex),
        HS = 1 << core::asInteger(ShaderStage::Domain),
        DS = 1 << core::asInteger(ShaderStage::Hull),
        GS = 1 << core::asInteger(ShaderStage::Geometry),
        PS = 1 << core::asInteger(ShaderStage::Pixel),
        CS = 1 << core::asInteger(ShaderStage::Compute),

        All = VS | HS | DS | GS | PS | CS
    };
    //ENUMFLAGS(ShaderStageFlags);
}