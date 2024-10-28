#pragma once

#if _MSAA2X 
#define SAMPLE_COUNT 2
#elif _MSAA4X
#define SAMPLE_COUNT 4
#elif _MSAA8X
#define SAMPLE_COUNT 8
#elif _MSAA16X
#define SAMPLE_COUNT 16
#else
#define SAMPLE_COUNT 1
#endif

int2 getMSAASampleScale(uint _sampleCount)
{
    switch(_sampleCount)
    {
        default: return int2(1,1);
        case 2:  return int2(2,1);
        case 4:  return int2(2,2);
        case 8:  return int2(4,2);
        case 16: return int2(4,4);
    }
}

int2 getMSAASampleOffset(uint _sampleCount, uint _sampleIndex)
{
    switch (_sampleCount)
    {
        case 2:
            switch (_sampleIndex)
            {
                case 0: return int2(0, 0);
                case 1: return int2(1, 0);
            }
            break;

        case 4:
            switch (_sampleIndex)
            {
                case 0: return int2(0, 0);
                case 1: return int2(1, 0);
                case 2: return int2(0, 1);
                case 3: return int2(1, 1);
            }
            break;

        case 8:
            switch (_sampleIndex)
            {
                case 0: return int2(0, 0);
                case 1: return int2(1, 0);
                case 2: return int2(2, 0);
                case 3: return int2(3, 0);
                case 4: return int2(0, 1);
                case 5: return int2(1, 1);
                case 6: return int2(2, 1);
                case 7: return int2(3, 1);
            }
            break;

        case 16:
            switch (_sampleIndex)
            {
                case 0: return int2(0, 0);
                case 1: return int2(1, 0);
                case 2: return int2(2, 0);
                case 3: return int2(3, 0);
                case 4: return int2(0, 1);
                case 5: return int2(1, 1);
                case 6: return int2(2, 1);
                case 7: return int2(3, 1);
                case 8: return int2(0, 2);
                case 9: return int2(1, 2);
                case 10: return int2(2, 2);
                case 11: return int2(3, 2);
                case 12: return int2(0, 3);
                case 13: return int2(1, 3);
                case 14: return int2(2, 3);
                case 15: return int2(3, 3);
            }
            break;
    }

    return (int2)0;
}

float4 getMSAASampleColor(uint _sampleIndex)
{
    switch(_sampleIndex)
    {
        default:
            return float4(0,0,0, 1);

        case 0:
            return float4(1,0,0, 1);

        #if SAMPLE_COUNT >= 2
        case 1:
            return float4(0,1,0, 1);

        #if SAMPLE_COUNT >= 4
        case 2:
            return float4(0,0,1, 1);

        case 3:
            return float4(1,1,1, 1);

        #if SAMPLE_COUNT >= 8
        case 4:
            return float4(0.75,0,0, 1);

        case 5:
            return float4(0,0.75,0, 1);

        case 6:
            return float4(0,0,0.75, 1);

        case 7:
            return float4(0.75,0.75,0.75, 1);

        #if SAMPLE_COUNT >= 16

        case 8:
            return float4(0.5,0,0, 1);

        case 9:
            return float4(0,0.5,0, 1);

        case 10:
            return float4(0,0,0.5, 1);

        case 11:
            return float4(0.5,0.5,0.5, 1);

        case 12:
            return float4(0.25,0,0, 1);

        case 13:
            return float4(0,0.25,0, 1);

        case 14:
            return float4(0,0,0.25, 1);

        case 15:
            return float4(0.25,0.25,0.25, 1);
        #endif // #if SAMPLE_COUNT >= 16
        #endif // SAMPLE_COUNT >= 8
        #endif // SAMPLE_COUNT >= 4
        #endif // SAMPLE_COUNT >= 2
    }
}