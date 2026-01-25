#include "OutlineOptions.h"

#if !VG_ENABLE_INLINE
#include "OutlineOptions.inl"
#endif

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // CategoryOptions
    //--------------------------------------------------------------------------------------

    VG_REGISTER_OBJECT_CLASS(OutlineOptions, "Category Options");

    declareResizeVectorDefaultFunc(OutlineOptions);

    //--------------------------------------------------------------------------------------
    bool OutlineOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(OutlineOptions, m_name, PropertyFlags::Hidden, false);

        registerPropertyEx(OutlineOptions, m_zPassOutlineColor, "ZPass", PropertyFlags::Color);
        registerPropertyEx(OutlineOptions, m_zFailOutlineColor, "ZFail", PropertyFlags::Color);

        registerResizeVectorDefaultFunc(OutlineOptions);

        return true;
    }

    //--------------------------------------------------------------------------------------
    OutlineOptions::OutlineOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    OutlineOptions::~OutlineOptions()
    {

    }

    //--------------------------------------------------------------------------------------
    // CategoryOptionsList
    //--------------------------------------------------------------------------------------

    VG_REGISTER_OBJECT_CLASS(OutlineOptionsList, "Category Options List");

    //--------------------------------------------------------------------------------------
    bool OutlineOptionsList::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }
}
