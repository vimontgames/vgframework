#include VG_GFXAPI_IMPL(SamplerState)

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    SamplerState::SamplerState(Sampler _sampler)
    {
        switch (_sampler)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(_sampler);

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

            case Sampler::AnisotropicClamp:
                *this = SamplerState(Filter::Linear, Address::Clamp, Anisotropy::Anisotropy_16X);
                break;

            case Sampler::AnisotropicRepeat:
                *this = SamplerState(Filter::Linear, Address::Repeat, Anisotropy::Anisotropy_16X);
                break;

            case Sampler::ShadowCmp:
                *this = SamplerState(Filter::DepthCmp, Address::Clamp);
                break;
        }
    }

    //--------------------------------------------------------------------------------------
    SamplerState::SamplerState(Filter  _filter, Address _address, Anisotropy _anisotropy)
    {
        filter = _filter;
        address = _address;
        anisotropy = _anisotropy;
    }
}