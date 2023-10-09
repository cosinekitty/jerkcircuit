#pragma once
#include <memory>
#include <vector>
#include "ChaoticOscillator.hpp"

namespace Analog
{
    extern const std::vector<const char *> ChaoticOscillatorKinds;
    std::unique_ptr<ChaoticOscillator> MakeChaoticOscillator(const char *kind);
}
