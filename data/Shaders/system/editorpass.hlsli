//#ifndef _EDITOR_PASS__HLSLI_
//#define _EDITOR_PASS__HLSLI_
//
//#include "types.hlsli"
//#include "packing.hlsli"
//#include "displaymodes.hlsli"
//
//struct EditorPassConstants
//{
//    #ifdef __cplusplus
//    EditorPassConstants()
//    {
//        m_data = 0;
//        setOutlineMask(BINDLESS_TEXTURE_INVALID);
//    }
//    #else
//    void Load(ByteAddressBuffer _buffer, uint _offset = 0)
//    {
//        m_data = _buffer.Load<EditorPassConstants>(_offset).m_data;
//    }
//    #endif
//
//    uint    m_data;    
//    
//    void    setOutlineMask  (uint _outlineMask) { m_data = packUint16low(m_data, _outlineMask); }
//    uint    getOutlineMask  ()                  { return unpackUint16low(m_data); }
//};
//
//#endif // _EDITOR_PASS__HLSLI_