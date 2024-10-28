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