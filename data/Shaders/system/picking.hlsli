#pragma once

#include "types.hlsli"

#define PICKING_MAX_HITS 16

struct PickingHit
{
    uint4 m_id;
    float4 m_pos;
};

struct PickingData
{
    uint4       m_counter;
    PickingHit  m_hits[PICKING_MAX_HITS];
};


