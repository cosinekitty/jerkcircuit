#include <cstring>
#include "MakeChaoticOscillator.hpp"

namespace Analog
{
    std::unique_ptr<ChaoticOscillator> MakeChaoticOscillator(const char *kind)
    {
        if (kind == nullptr)
            return nullptr;

        if (!strcmp(kind, "ruck"))
            return std::make_unique<Rucklidge>();

        return nullptr;
    }
}
