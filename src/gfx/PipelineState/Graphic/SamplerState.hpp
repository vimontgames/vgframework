#include VG_GFXAPI_IMPL(SamplerState)

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    SamplerState::SamplerState(Sampler _sampler)
    {
        switch (_sampler)
        {
            default:
                VG_ASSERT(false, "Unhandled Sampler \"%s\" (%u)", asString(_sampler).c_str());
            case Sampler::NearestClamp:
                *this = SamplerState(Filter::Nearest, Address::Clamp);
                break;
            case Sampler::NearestRepeat:
                *this = SamplerState(Filter::Nearest, Address::Repeat);
                break;
            case Sampler::LinearClamp:
                *this = SamplerState(Filter::Linear, Address::Clamp);
                break;
            case Sampler::LinearRepeat:
                *this = SamplerState(Filter::Linear, Address::Repeat);
                break;
        }
    }

    //--------------------------------------------------------------------------------------
    SamplerState::SamplerState(Filter  _filter, Address _address)
    {
        filter = _filter;
        address = _address;
    }
}