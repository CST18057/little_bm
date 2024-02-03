#include <benchmark/benchmark.h>
#include <cstdint>
// taskset -c 0  ./hamming_weight
#define BM(x) BENCHMARK(BM_##x)

#include <algorithm>
#include <random>
#include <vector>

std::vector<uint64_t> createTestNumbersV0() {
  std::vector<uint64_t> v{
    7,
    47,
    223,
    3967,
    16127,
    1046527,
    16769023,
    1073676287,
    68718952447,
    274876858367,
    4398042316799,
    1125899839733759,
    18014398241046527};
  return v;
}

std::vector<uint64_t> createTestNumbers(int n) {
  if (n == 0) {
    return createTestNumbersV0();
  }
  std::vector<uint64_t> v;
  std::random_device rd;
  std::uniform_int_distribution<uint64_t> d(0, std::numeric_limits<uint64_t>::max());
  for (int i = 0; i < n; ++i) v.emplace_back(d(rd));
  return v;
}
int hammingWeightV2(uint64_t n) {
    n -= (n >> 1) & 0x5555555555555555ULL;
    n = ((n >> 2) & 0x3333333333333333ULL) + (n & 0x3333333333333333ULL);
    return (((n + (n >> 4)) & 0xF0F0F0F0F0F0F0FULL)
            * 0x101010101010101ULL) >> 56;
}

int hammingWeight(uint64_t n) {
    int ret = 0;
    while (n) {
        n &= n - 1;
        ret++;
    }
    return ret;
}

int hammingWeight_popcnt(uint64_t n) {
  return __builtin_popcountll(n);
}

/*
int hammingWeight_popcntV2(uint64_t n) {
    int32_t count = 0;
    asm("popcnt %1,%0" : "=r"(count) : "rm"(n) : "cc");
    return count;
}

void BM_hammingWeight_popcntV2(benchmark::State& state) {
  auto numbers = createTestNumbers();
  for (auto _ : state) {
    for (auto v : numbers) 
        benchmark::DoNotOptimize(hammingWeight_popcntV2(v));
  }
}
*/
void BM_hammingWeight_popcnt(benchmark::State& state) {
  auto numbers = createTestNumbers(state.range(0));
  for (auto _ : state) {
    for (auto v : numbers) 
        benchmark::DoNotOptimize(hammingWeight_popcnt(v));
  }
}

void BM_hammingWeightV2(benchmark::State& state) {
  auto numbers = createTestNumbers(state.range(0));
  for (auto _ : state) {
    for (auto v : numbers) 
        benchmark::DoNotOptimize(hammingWeightV2(v));
  }
}


void BM_hammingWeight(benchmark::State& state) {
  auto numbers = createTestNumbers(state.range(0));
  for (auto _ : state) {
    for (auto v : numbers) 
        benchmark::DoNotOptimize(hammingWeight(v));
  }
}

//BM(hammingWeight_popcntV2);
BM(hammingWeight_popcnt)->Args({0})
                        ->Args({128})
                        ->Args({256})
                        ->Args({512})
                        ->Args({1024});

BM(hammingWeight)->Args({0})
                        ->Args({128})
                        ->Args({256})
                        ->Args({512})
                        ->Args({1024});

BM(hammingWeightV2)->Args({0, 0})
                        ->Args({128})
                        ->Args({256})
                        ->Args({512})
                        ->Args({1024});



BENCHMARK_MAIN();

/*

taskset -c 0  ./hamming_weight
2024-02-03T22:39:35+08:00
Running ./hamming_weight
Run on (16 X 3392.38 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 16384 KiB (x1)
Load Average: 0.01, 0.02, 0.00
-----------------------------------------------------------------------
Benchmark                             Time             CPU   Iterations
-----------------------------------------------------------------------
BM_hammingWeight_popcnt/0          26.6 ns         26.6 ns     26281110
BM_hammingWeight_popcnt/128         265 ns          265 ns      2622989
BM_hammingWeight_popcnt/256         526 ns          526 ns      1332695
BM_hammingWeight_popcnt/512        1048 ns         1048 ns       666562
BM_hammingWeight_popcnt/1024       2096 ns         2095 ns       334434
BM_hammingWeight/0                 80.7 ns         80.7 ns      8689750
BM_hammingWeight/128               1643 ns         1642 ns       447638
BM_hammingWeight/256               3646 ns         3646 ns       195882
BM_hammingWeight/512               8099 ns         8097 ns        85508
BM_hammingWeight/1024             17193 ns        17190 ns        41208
BM_hammingWeightV2/0/0             11.8 ns         11.8 ns     58402778
BM_hammingWeightV2/128              118 ns          118 ns      5951445
BM_hammingWeightV2/256              233 ns          233 ns      3001257
BM_hammingWeightV2/512              463 ns          463 ns      1510925
BM_hammingWeightV2/1024             927 ns          927 ns       758631

*/