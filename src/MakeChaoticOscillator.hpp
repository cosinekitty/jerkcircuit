#pragma once
#include <memory>
#include "ChaoticOscillator.hpp"

namespace Analog
{
    std::unique_ptr<ChaoticOscillator> MakeChaoticOscillator(const char *kind);
}
