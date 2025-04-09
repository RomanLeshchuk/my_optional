#ifndef MEASURE_PERFORMANCE_H
#define MEASURE_PERFORMANCE_H

#include <chrono>
#include <algorithm>
#include <vector>

template <typename Optional>
double measure_optional_performance(size_t iters)
{
    auto startTime = std::chrono::steady_clock::now();

    std::vector<Optional> vals(iters);
    for (size_t i = 0; i < iters; i++)
    {
        vals[i] = iters - i;
    }

    std::sort(vals.begin(), vals.end());

    for (size_t i = 1; i < iters; i++)
    {
        // not using and_then, because it only appeared in C++23 in standard std::optional
        *vals[i] += *vals[i - 1];
    }

    double resTime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - startTime).count()) / 1000.0;

    assert(vals.back() == iters * (iters + 1) / 2);

    return resTime;
}

#endif
