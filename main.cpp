#include <optional>
#include <cmath>
#include <iostream>

#include "test.h"
#include "measure_performance.h"
#include "my_optional.h"

/*
My optional is averagely 10 percents slower than standard on my machine (in release mode),
that may be because of not optimal if/else statements and maybe because of using
buffer array approach, while most compilers are using union.
*/

int main()
{
    test();

    std::cout << "Correctness tests passed (if run in debug mode), measuring performance..." << std::endl;

    size_t performance_test_iters = 100000000;

    double standard_optional_perf = measure_optional_performance<std::optional<size_t>>(performance_test_iters);
    std::cout << "Standard optional performance: " << standard_optional_perf << " ms" << std::endl;

    double my_optional_perf = measure_optional_performance<my_optional<size_t>>(performance_test_iters);
    std::cout << "My optional performance: " << my_optional_perf << " ms" << std::endl;

    std::cout << "My optional is " << std::round(
        (my_optional_perf / standard_optional_perf - 1) * 10000
        ) / 100 << " percents slower than standard" << std::endl;

    return 0;
}
