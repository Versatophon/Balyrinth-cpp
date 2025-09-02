#pragma once

#include <cstdint>

struct IVec2
{
    union {
        int32_t X;
        int32_t Width;
    };
    union {
        int32_t Y;
        int32_t Height;
    };
};
