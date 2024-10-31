#include "benchmark/benchmark.h"
#include <bits/elements_of.h>
#include <cstddef>
#include <span>
#include <string>
#include <coroutine>
#include <type_traits>
#include <generator>
#include <string_view>
#include <vector>
#include "generator.hpp"

#ifdef TEST_SHORT
std::string_view sv{"test"};
#elif defined (TEST_MIDDILE)
std::string_view sv{"test-test-test-test"};
#elif defined (TEST_LONG)
std::string_view sv{"test-test-test-test-test-test-test-test-test-test-test-test"};
#else
std::string_view sv{"test"};
#endif

template <size_t N>
std::generator<const char> gen1(std::string_view v) {
    if constexpr (N > 0) {
        co_yield std::ranges::elements_of(gen1<N - 1>(v));
    } else {
        for (auto c : v) {
            co_yield c;
        }
    }
}
template <size_t N>
std::generator<const char> gen2(std::string_view v) {
    if constexpr (N > 0) {
        for (auto c : gen2<N - 1>(v)) {
            co_yield c;
        }
    } else {
        for (auto c : v) {
            co_yield c;
        }
    }
}
template <size_t N>
oeo::generator<const char> gen3(std::string_view v) {
    if constexpr (N > 0) {
        for (auto c : gen3<N - 1>(v)) {
            co_yield c;
        }
    } else {
        for (auto c : v) {
            co_yield c;
        }
    }
}

template <size_t N>
std::generator<const char> f_gen_elements_of(std::string_view v) {
    static_assert(N > 0);
    co_yield std::ranges::elements_of(gen1<N - 1>(v));
}

template <size_t N>
std::generator<const char> f_gen_for_gen(std::string_view v) {
    static_assert(N > 0);
    for (auto x : gen2<N - 1>(v)) {
        co_yield x;
    }
}

template <size_t N>
oeo::generator<const char> f_oeo_gen_for_gen(std::string_view v) {
    static_assert(N > 0);
    for (auto x : gen3<N - 1>(v)) {
        co_yield x;
    }
}

template <size_t N>
static void GenGen(benchmark::State& state) {
    for (auto _ : state) {
        int sum = 0;
        benchmark::DoNotOptimize(sv);
        benchmark::DoNotOptimize(sum);
        if constexpr (N == 0) {
            for (auto x : sv) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else if constexpr (N == 1) {
            for (auto x : gen1<0>(sv)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else {
            for (auto x : f_gen_elements_of<N - 1>(sv)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_TEMPLATE(GenGen, 0);
BENCHMARK_TEMPLATE(GenGen, 1);
BENCHMARK_TEMPLATE(GenGen, 2);
BENCHMARK_TEMPLATE(GenGen, 3);
BENCHMARK_TEMPLATE(GenGen, 5);
BENCHMARK_TEMPLATE(GenGen, 10);
BENCHMARK_TEMPLATE(GenGen, 20);
BENCHMARK_TEMPLATE(GenGen, 50);
BENCHMARK_TEMPLATE(GenGen, 100);

template <size_t N>
static void GenFor(benchmark::State& state) {
    for (auto _ : state) {
        int sum = 0;
        benchmark::DoNotOptimize(sv);
        benchmark::DoNotOptimize(sum);
        if constexpr (N == 0) {
            for (auto x : sv) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else if constexpr (N == 1) {
            for (auto x : gen2<0>(sv)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else {
            for (auto x : f_gen_for_gen<N - 1>(sv)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        }
    }
}
BENCHMARK_TEMPLATE(GenFor, 0);
BENCHMARK_TEMPLATE(GenFor, 1);
BENCHMARK_TEMPLATE(GenFor, 2);
BENCHMARK_TEMPLATE(GenFor, 3);
BENCHMARK_TEMPLATE(GenFor, 5);
BENCHMARK_TEMPLATE(GenFor, 10);
BENCHMARK_TEMPLATE(GenFor, 20);
BENCHMARK_TEMPLATE(GenFor, 50);
BENCHMARK_TEMPLATE(GenFor, 100);

template <size_t N>
static void OEOGenFor(benchmark::State& state) {
    for (auto _ : state) {
        int sum = 0;
        benchmark::DoNotOptimize(sv);
        benchmark::DoNotOptimize(sum);
        if constexpr (N == 0) {
            for (auto x : sv) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else if constexpr (N == 1) {
            for (auto x : gen3<0>(sv)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else {
            for (auto x : f_oeo_gen_for_gen<N - 1>(sv)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        }
    }
}
BENCHMARK_TEMPLATE(OEOGenFor, 0);
BENCHMARK_TEMPLATE(OEOGenFor, 1);
BENCHMARK_TEMPLATE(OEOGenFor, 2);
BENCHMARK_TEMPLATE(OEOGenFor, 3);
BENCHMARK_TEMPLATE(OEOGenFor, 5);
BENCHMARK_TEMPLATE(OEOGenFor, 10);
BENCHMARK_TEMPLATE(OEOGenFor, 20);
BENCHMARK_TEMPLATE(OEOGenFor, 50);
BENCHMARK_TEMPLATE(OEOGenFor, 100);

static void TrivialGenFor(benchmark::State& state) {
    for (auto _ : state) {
        int sum = 0;
        benchmark::DoNotOptimize(sum);
        for (auto x : sv) {
            sum += x;
            benchmark::DoNotOptimize(x);
        }
        benchmark::DoNotOptimize(sv);
    }
}
BENCHMARK(TrivialGenFor);

std::generator<const char> gen1(std::string_view v, size_t N) {
    if (N > 0) {
        co_yield std::ranges::elements_of(gen1(v, N - 1));
    } else {
        for (auto c : v) {
            co_yield c;
        }
    }
}
std::generator<const char> gen2(std::string_view v, size_t N) {
    if (N > 0) {
        for (auto c : gen2(v, N - 1)) {
            co_yield c;
        }
    } else {
        for (auto c : v) {
            co_yield c;
        }
    }
}
oeo::generator<const char> gen3(std::string_view v, size_t N) {
    if (N > 0) {
        for (auto c : gen3(v, N - 1)) {
            co_yield c;
        }
    } else {
        for (auto c : v) {
            co_yield c;
        }
    }
}

std::generator<const char> f_gen_elements_of(std::string_view v, size_t N) {
    co_yield std::ranges::elements_of(gen1(v, N - 1));
}

std::generator<const char> f_gen_for_gen(std::string_view v, size_t N) {
    for (auto x : gen2(v, N - 1)) {
        co_yield x;
    }
}

oeo::generator<const char> f_oeo_gen_for_gen(std::string_view v, size_t N) {
    for (auto x : gen3(v, N - 1)) {
        co_yield x;
    }
}

static void DynGenGen(benchmark::State& state) {
    size_t N = state.range(0);
    for (auto _ : state) {
        int sum = 0;
        benchmark::DoNotOptimize(sv);
        benchmark::DoNotOptimize(sum);
        if (N == 0) {
            for (auto x : sv) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else if (N == 1) {
            for (auto x : gen1(sv, 0)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else {
            for (auto x : f_gen_elements_of(sv, N - 1)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK(DynGenGen)->Arg(0)
->Arg(1)
->Arg(2)
->Arg(3)
->Arg(5)
->Arg(10)
->Arg(20)
->Arg(50)
->Arg(100)
;

static void DynGenFor(benchmark::State& state) {
    size_t N = state.range(0);
    for (auto _ : state) {
        int sum = 0;
        benchmark::DoNotOptimize(sv);
        benchmark::DoNotOptimize(sum);
        if (N == 0) {
            for (auto x : sv) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else if (N == 1) {
            for (auto x : gen2(sv, 0)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else {
            for (auto x : f_gen_for_gen(sv, N - 1)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        }
    }
}
BENCHMARK(DynGenFor)->Arg(0)
->Arg(1)
->Arg(2)
->Arg(3)
->Arg(5)
->Arg(10)
->Arg(20)
->Arg(50)
->Arg(100)
;

static void DynOEOGenFor(benchmark::State& state) {
    size_t N = state.range(0);
    for (auto _ : state) {
        int sum = 0;
        benchmark::DoNotOptimize(sv);
        benchmark::DoNotOptimize(sum);
        if (N == 0) {
            for (auto x : sv) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else if  (N == 1) {
            for (auto x : gen3(sv, 0)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        } else {
            for (auto x : f_oeo_gen_for_gen(sv, N - 1)) {
                sum += x;
                benchmark::DoNotOptimize(x);
            }
        }
    }
}
BENCHMARK(DynOEOGenFor)->Arg(0)
->Arg(1)
->Arg(2)
->Arg(3)
->Arg(5)
->Arg(10)
->Arg(20)
->Arg(50)
->Arg(100)
;
BENCHMARK_MAIN();