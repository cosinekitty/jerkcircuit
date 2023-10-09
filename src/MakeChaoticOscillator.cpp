#include <cstring>
#include "MakeChaoticOscillator.hpp"

namespace Analog
{
    const std::vector<const char *> ChaoticOscillatorKinds
    {
        "aiza",
        "ruck",
        "sprot"
    };

    std::unique_ptr<ChaoticOscillator> MakeChaoticOscillator(const char *kind)
    {
        if (kind == nullptr)
            return nullptr;

        if (!strcmp(kind, "aiza"))
            return std::make_unique<Aizawa>();

        if (!strcmp(kind, "ruck"))
            return std::make_unique<Rucklidge>();

        if (!strcmp(kind, "sprot"))
            return std::make_unique<Sprott>();

        return nullptr;
    }
}
