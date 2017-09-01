#pragma once

#include "Base.hpp"

#include <chrono>
#include <vector>
#include <iostream>

namespace kepler {

    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

    class ProfileBlock final {
    public:
        explicit ProfileBlock(size_t index) : _index(index) {
            _t1 = std::chrono::high_resolution_clock::now();
        }
        ~ProfileBlock() {
            _t2 = std::chrono::high_resolution_clock::now();
            s_totals[_index] += _t2 - _t1;
            ++s_counts[_index];
        }

        static void printTime(size_t index) {
            std::cout << s_totals[index].count() << " ns" << std::endl;
            auto ns = static_cast<double>(s_totals[index].count());
            auto t = ns / 1000000.0;
            std::cout << t << " ms" << std::endl;
            if (s_counts[index] > 0) {
                auto avg = ns / static_cast<double>(s_counts[index]);
                std::cout << avg << " ns avg" << std::endl;
            }
        }

    private:
        static std::vector<std::chrono::nanoseconds> s_totals;
        static std::vector<size_t> s_counts;
        size_t _index;
        TimePoint _t1;
        TimePoint _t2;
    };
}
