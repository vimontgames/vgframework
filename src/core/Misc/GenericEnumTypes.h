#pragma once

namespace vg::core
{
    #define vg_generic_enum_value_8(Namespace, enumTypename) vg_enum_class(Namespace, enumTypename, vg::core::u8,     \
        enumTypename##_0,                                                                                             \
        enumTypename##_1,                                                                                             \
        enumTypename##_2,                                                                                             \
        enumTypename##_3,                                                                                             \
        enumTypename##_4,                                                                                             \
        enumTypename##_5,                                                                                             \
        enumTypename##_6,                                                                                             \
        enumTypename##_7                                                                                              \
    );

    #define vg_generic_enum_value_16(Namespace, enumTypename) vg_enum_class(Namespace, enumTypename, vg::core::u8,    \
        enumTypename##_0,  enumTypename##_1,  enumTypename##_2,  enumTypename##_3,                                    \
        enumTypename##_4,  enumTypename##_5,  enumTypename##_6,  enumTypename##_7,                                    \
        enumTypename##_8,  enumTypename##_9,  enumTypename##_10, enumTypename##_11,                                   \
        enumTypename##_12, enumTypename##_13, enumTypename##_14, enumTypename##_15                                    \
    );

    #define vg_generic_enum_value_32(Namespace, enumTypename) vg_enum_class(Namespace, enumTypename, vg::core::u8,    \
        enumTypename##_0,  enumTypename##_1,  enumTypename##_2,  enumTypename##_3,                                    \
        enumTypename##_4,  enumTypename##_5,  enumTypename##_6,  enumTypename##_7,                                    \
        enumTypename##_8,  enumTypename##_9,  enumTypename##_10, enumTypename##_11,                                   \
        enumTypename##_12, enumTypename##_13, enumTypename##_14, enumTypename##_15,                                   \
        enumTypename##_16, enumTypename##_17, enumTypename##_18, enumTypename##_19,                                   \
        enumTypename##_20, enumTypename##_21, enumTypename##_22, enumTypename##_23,                                   \
        enumTypename##_24, enumTypename##_25, enumTypename##_26, enumTypename##_27,                                   \
        enumTypename##_28, enumTypename##_29, enumTypename##_30, enumTypename##_31                                    \
    );

    #define vg_generic_enum_value_64(Namespace, enumTypename) vg_enum_class(Namespace, enumTypename, vg::core::u8,    \
        enumTypename##_0,  enumTypename##_1,  enumTypename##_2,  enumTypename##_3,                                    \
        enumTypename##_4,  enumTypename##_5,  enumTypename##_6,  enumTypename##_7,                                    \
        enumTypename##_8,  enumTypename##_9,  enumTypename##_10, enumTypename##_11,                                   \
        enumTypename##_12, enumTypename##_13, enumTypename##_14, enumTypename##_15,                                   \
        enumTypename##_16, enumTypename##_17, enumTypename##_18, enumTypename##_19,                                   \
        enumTypename##_20, enumTypename##_21, enumTypename##_22, enumTypename##_23,                                   \
        enumTypename##_24, enumTypename##_25, enumTypename##_26, enumTypename##_27,                                   \
        enumTypename##_28, enumTypename##_29, enumTypename##_30, enumTypename##_31,                                   \
        enumTypename##_32, enumTypename##_33, enumTypename##_34, enumTypename##_35,                                   \
        enumTypename##_36, enumTypename##_37, enumTypename##_38, enumTypename##_39,                                   \
        enumTypename##_40, enumTypename##_41, enumTypename##_42, enumTypename##_43,                                   \
        enumTypename##_44, enumTypename##_45, enumTypename##_46, enumTypename##_47,                                   \
        enumTypename##_48, enumTypename##_49, enumTypename##_50, enumTypename##_51,                                   \
        enumTypename##_52, enumTypename##_53, enumTypename##_54, enumTypename##_55,                                   \
        enumTypename##_56, enumTypename##_57, enumTypename##_58, enumTypename##_59,                                   \
        enumTypename##_60, enumTypename##_61, enumTypename##_62, enumTypename##_63                                    \
    );

    #define vg_generic_enum_bitfield_8(Namespace, enumTypename) vg_enum_class(Namespace, enumTypename, vg::core::u8,    \
        enumTypename##_0  = 0x01,                                                                                       \
        enumTypename##_1  = 0x02,                                                                                       \
        enumTypename##_2  = 0x04,                                                                                       \
        enumTypename##_3  = 0x08,                                                                                       \
        enumTypename##_4  = 0x10,                                                                                       \
        enumTypename##_5  = 0x20,                                                                                       \
        enumTypename##_6  = 0x40,                                                                                       \
        enumTypename##_7  = 0x80                                                                                        \
    );

    #define vg_generic_enum_bitfield_16(Namespace, enumTypename) vg_enum_class(Namespace, enumTypename, vg::core::u16,                              \
        enumTypename##_0  = 0x00000001ULL, enumTypename##_1  = 0x00000002ULL, enumTypename##_2  = 0x00000004ULL, enumTypename##_3  = 0x00000008ULL, \
        enumTypename##_4  = 0x00000010ULL, enumTypename##_5  = 0x00000020ULL, enumTypename##_6  = 0x00000040ULL, enumTypename##_7  = 0x00000080ULL, \
        enumTypename##_8  = 0x00000100ULL, enumTypename##_9  = 0x00000200ULL, enumTypename##_10 = 0x00000400ULL, enumTypename##_11 = 0x00000800ULL, \
        enumTypename##_12 = 0x00001000ULL, enumTypename##_13 = 0x00002000ULL, enumTypename##_14 = 0x00004000ULL, enumTypename##_15 = 0x00008000ULL  \
    );

    #define vg_generic_enum_bitfield_32(Namespace, enumTypename) vg_enum_class(Namespace, enumTypename, vg::core::u32,                              \
        enumTypename##_0  = 0x00000001ULL, enumTypename##_1  = 0x00000002ULL, enumTypename##_2  = 0x00000004ULL, enumTypename##_3  = 0x00000008ULL, \
        enumTypename##_4  = 0x00000010ULL, enumTypename##_5  = 0x00000020ULL, enumTypename##_6  = 0x00000040ULL, enumTypename##_7  = 0x00000080ULL, \
        enumTypename##_8  = 0x00000100ULL, enumTypename##_9  = 0x00000200ULL, enumTypename##_10 = 0x00000400ULL, enumTypename##_11 = 0x00000800ULL, \
        enumTypename##_12 = 0x00001000ULL, enumTypename##_13 = 0x00002000ULL, enumTypename##_14 = 0x00004000ULL, enumTypename##_15 = 0x00008000ULL, \
        enumTypename##_16 = 0x00010000ULL, enumTypename##_17 = 0x00020000ULL, enumTypename##_18 = 0x00040000ULL, enumTypename##_19 = 0x00080000ULL, \
        enumTypename##_20 = 0x00100000ULL, enumTypename##_21 = 0x00200000ULL, enumTypename##_22 = 0x00400000ULL, enumTypename##_23 = 0x00800000ULL, \
        enumTypename##_24 = 0x01000000ULL, enumTypename##_25 = 0x02000000ULL, enumTypename##_26 = 0x04000000ULL, enumTypename##_27 = 0x08000000ULL, \
        enumTypename##_28 = 0x10000000ULL, enumTypename##_29 = 0x20000000ULL, enumTypename##_30 = 0x40000000ULL, enumTypename##_31 = 0x80000000ULL  \
    );

    #define vg_generic_enum_bitfield_64(Namespace, enumTypename) vg_enum_class(Namespace, enumTypename, vg::core::u64,                                                              \
        enumTypename##_0  = 0x0000000000000001ULL, enumTypename##_1  = 0x0000000000000002ULL, enumTypename##_2  = 0x0000000000000004ULL, enumTypename##_3  = 0x0000000000000008ULL, \
        enumTypename##_4  = 0x0000000000000010ULL, enumTypename##_5  = 0x0000000000000020ULL, enumTypename##_6  = 0x0000000000000040ULL, enumTypename##_7  = 0x0000000000000080ULL, \
        enumTypename##_8  = 0x0000000000000100ULL, enumTypename##_9  = 0x0000000000000200ULL, enumTypename##_10 = 0x0000000000000400ULL, enumTypename##_11 = 0x0000000000000800ULL, \
        enumTypename##_12 = 0x0000000000001000ULL, enumTypename##_13 = 0x0000000000002000ULL, enumTypename##_14 = 0x0000000000004000ULL, enumTypename##_15 = 0x0000000000008000ULL, \
        enumTypename##_16 = 0x0000000000010000ULL, enumTypename##_17 = 0x0000000000020000ULL, enumTypename##_18 = 0x0000000000040000ULL, enumTypename##_19 = 0x0000000000080000ULL, \
        enumTypename##_20 = 0x0000000000100000ULL, enumTypename##_21 = 0x0000000000200000ULL, enumTypename##_22 = 0x0000000000400000ULL, enumTypename##_23 = 0x0000000000800000ULL, \
        enumTypename##_24 = 0x0000000001000000ULL, enumTypename##_25 = 0x0000000002000000ULL, enumTypename##_26 = 0x0000000004000000ULL, enumTypename##_27 = 0x0000000008000000ULL, \
        enumTypename##_28 = 0x0000000010000000ULL, enumTypename##_29 = 0x0000000020000000ULL, enumTypename##_30 = 0x0000000040000000ULL, enumTypename##_31 = 0x0000000080000000ULL, \
        enumTypename##_32 = 0x0000000100000000ULL, enumTypename##_33 = 0x0000000200000000ULL, enumTypename##_34 = 0x0000000400000000ULL, enumTypename##_35 = 0x0000000800000000ULL, \
        enumTypename##_36 = 0x0000001000000000ULL, enumTypename##_37 = 0x0000002000000000ULL, enumTypename##_38 = 0x0000004000000000ULL, enumTypename##_39 = 0x0000008000000000ULL, \
        enumTypename##_40 = 0x0000010000000000ULL, enumTypename##_41 = 0x0000020000000000ULL, enumTypename##_42 = 0x0000040000000000ULL, enumTypename##_43 = 0x0000080000000000ULL, \
        enumTypename##_44 = 0x0000100000000000ULL, enumTypename##_45 = 0x0000200000000000ULL, enumTypename##_46 = 0x0000400000000000ULL, enumTypename##_47 = 0x0000800000000000ULL, \
        enumTypename##_48 = 0x0001000000000000ULL, enumTypename##_49 = 0x0002000000000000ULL, enumTypename##_50 = 0x0004000000000000ULL, enumTypename##_51 = 0x0008000000000000ULL, \
        enumTypename##_52 = 0x0010000000000000ULL, enumTypename##_53 = 0x0020000000000000ULL, enumTypename##_54 = 0x0040000000000000ULL, enumTypename##_55 = 0x0080000000000000ULL, \
        enumTypename##_56 = 0x0100000000000000ULL, enumTypename##_57 = 0x0200000000000000ULL, enumTypename##_58 = 0x0400000000000000ULL, enumTypename##_59 = 0x0800000000000000ULL, \
        enumTypename##_60 = 0x1000000000000000ULL, enumTypename##_61 = 0x2000000000000000ULL, enumTypename##_62 = 0x4000000000000000ULL, enumTypename##_63 = 0x8000000000000000ULL  \
    );
}

