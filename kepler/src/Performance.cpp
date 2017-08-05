#include "stdafx.h"
#include "Performance.hpp"

namespace kepler {

    constexpr size_t SIZE = 10;

    std::vector<std::chrono::nanoseconds> ProfileBlock::s_totals(SIZE);
    std::vector<size_t> ProfileBlock::s_counts(SIZE);
}
