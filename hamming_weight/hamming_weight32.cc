#include <benchmark/benchmark.h>
#include <cstdint>
// taskset -c 0  ./bm_hamming_weight32
#define BM(x) BENCHMARK(BM_##x)
// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan


#include <algorithm>
#include <random>
#include <vector>

std::vector<uint32_t> createTestNumbersV0() {
  std::vector<uint32_t> v{
    7,
    47,
    223,
    3967,
    16127,
    1046527,
    16769023,
    1073676287};
  return v;
}
static const unsigned char BitsSetTable256[256] = 
{
#   define B2(n) n,     n+1,     n+1,     n+2
#   define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#   define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
    B6(0), B6(1), B6(1), B6(2)
};
/*
void buildSetTable() {
  // To initially generate the table algorithmically:
  BitsSetTable256[0] = 0;
  for (int i = 0; i < 256; i++)
  {
    BitsSetTable256[i] = (i & 1) + BitsSetTable256[i / 2];
  }
}
*/

std::vector<uint32_t> createTestNumbers(int n) {
  if (n == 0) {
    return createTestNumbersV0();
  }
  std::vector<uint32_t> v;
  std::random_device rd;
  std::uniform_int_distribution<uint32_t> d(0, std::numeric_limits<uint32_t>::max());
  for (int i = 0; i < n; ++i) v.emplace_back(d(rd));
  return v;
}

int hammingWeight_table0(uint32_t v) {
  unsigned char * p = (unsigned char *) &v;
  uint32_t c = BitsSetTable256[v & 0xff] + 
    BitsSetTable256[(v >> 8) & 0xff] + 
    BitsSetTable256[(v >> 16) & 0xff] + 
    BitsSetTable256[v >> 24]; 
  return c;
}

int hammingWeight_table(uint32_t v) {
  unsigned char * p = (unsigned char *) &v;
  uint32_t c = BitsSetTable256[p[0]] + 
      BitsSetTable256[p[1]] + 
      BitsSetTable256[p[2]] +	
      BitsSetTable256[p[3]];
  return c;
}

int hammingWeight_14(uint32_t v) {
  // option 1, for at most 14-bit values in v:
  return (v * 0x200040008001ULL & 0x111111111111111ULL) % 0xf;
}

int hammingWeight_24(uint32_t v) {
  unsigned int c; // c accumulates the total bits set in v
  // option 2, for at most 24-bit values in v:
  c =  ((v & 0xfff) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;
  c += (((v & 0xfff000) >> 12) * 0x1001001001001ULL & 0x84210842108421ULL) 
      % 0x1f;
  return c;
}

int hammingWeight_32(uint32_t v) {
  unsigned int c;
  // option 3, for at most 32-bit values in v:
  c =  ((v & 0xfff) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;
  c += (((v & 0xfff000) >> 12) * 0x1001001001001ULL & 0x84210842108421ULL) % 
      0x1f;
  c += ((v >> 24) * 0x1001001001001ULL & 0x84210842108421ULL) % 0x1f;
  return c;
}



int hammingWeightV2(uint32_t n) {
  uint32_t c; // store the total here
  static const int S[] = {1, 2, 4, 8, 16}; // Magic Binary Numbers
  static const int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF};

  c = n - ((n >> 1) & B[0]);
  c = ((c >> S[1]) & B[1]) + (c & B[1]);
  c = ((c >> S[2]) + c) & B[2];
  c = ((c >> S[3]) + c) & B[3];
  c = ((c >> S[4]) + c) & B[4];
  return c;
}

int hammingWeight(uint32_t n) {
    int ret = 0;
    while (n) {
        n &= n - 1;
        ret++;
    }
    return ret;
}

int hammingWeight_popcnt(uint32_t n) {
  return __builtin_popcount(n);
}



int hammingWeight_popcntV2(uint32_t n) {
    int32_t count = 0;
    asm("popcnt %1,%0" : "=r"(count) : "rm"(n) : "cc");
    return count;
}

void BM_hammingWeight_popcntV2(benchmark::State& state) {
  auto numbers = createTestNumbers(state.range(0));
  for (auto _ : state) {
    for (auto v : numbers) 
        benchmark::DoNotOptimize(hammingWeight_popcntV2(v));
  }
}

void BM_hammingWeight_table(benchmark::State& state) {
  auto numbers = createTestNumbers(state.range(0));
  for (auto _ : state) {
    for (auto v : numbers) 
        benchmark::DoNotOptimize(hammingWeight_table(v));
  }
}


void BM_hammingWeight_table0(benchmark::State& state) {
  auto numbers = createTestNumbers(state.range(0));
  for (auto _ : state) {
    for (auto v : numbers) 
        benchmark::DoNotOptimize(hammingWeight_table0(v));
  }
}

void BM_hammingWeight_32(benchmark::State& state) {
  auto numbers = createTestNumbers(state.range(0));
  for (auto _ : state) {
    for (auto v : numbers) 
        benchmark::DoNotOptimize(hammingWeight_32(v));
  }
}

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

BM(hammingWeight_popcnt)->Args({0})
                        ->Args({128})
                        ->Args({256})
                        ->Args({512})
                        ->Args({1024});

BM(hammingWeight_popcntV2)->Args({0})
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

BM(hammingWeight_32)->Args({0})
                        ->Args({128})
                        ->Args({256})
                        ->Args({512})
                        ->Args({1024});

BM(hammingWeight_table)->Args({0})
                        ->Args({128})
                        ->Args({256})
                        ->Args({512})
                        ->Args({1024});

BM(hammingWeight_table0)->Args({0})
                        ->Args({128})
                        ->Args({256})
                        ->Args({512})
                        ->Args({1024});
BENCHMARK_MAIN();

/*

Running ./bm_hamming_weight32
Run on (16 X 4846.01 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 16384 KiB (x1)
Load Average: 1.55, 0.63, 0.25
-------------------------------------------------------------------------
Benchmark                               Time             CPU   Iterations
-------------------------------------------------------------------------
BM_hammingWeight_popcnt/0            16.5 ns         16.5 ns     42383812
BM_hammingWeight_popcnt/128           267 ns          267 ns      2619166
BM_hammingWeight_popcnt/256           532 ns          532 ns      1324930
BM_hammingWeight_popcnt/512          1059 ns         1059 ns       663500
BM_hammingWeight_popcnt/1024         2109 ns         2109 ns       331816
BM_hammingWeight_popcntV2/0          1.85 ns         1.85 ns    377915646
BM_hammingWeight_popcntV2/128        35.3 ns         35.3 ns     19801503
BM_hammingWeight_popcntV2/256        67.9 ns         67.9 ns     10361112
BM_hammingWeight_popcntV2/512         128 ns          128 ns      5479843
BM_hammingWeight_popcntV2/1024        254 ns          254 ns      2754659
BM_hammingWeight/0                   26.8 ns         26.8 ns     26135510
BM_hammingWeight/128                  534 ns          534 ns      1338830
BM_hammingWeight/256                 1064 ns         1063 ns       650965
BM_hammingWeight/512                 2127 ns         2127 ns       317412
BM_hammingWeight/1024                5414 ns         5414 ns       119051
BM_hammingWeightV2/0/0               8.72 ns         8.72 ns     79976901
BM_hammingWeightV2/128                139 ns          139 ns      5036550
BM_hammingWeightV2/256                276 ns          276 ns      2535186
BM_hammingWeightV2/512                547 ns          547 ns      1266449
BM_hammingWeightV2/1024              1093 ns         1093 ns       637382
BM_hammingWeight_32/0                22.6 ns         22.6 ns     30942800
BM_hammingWeight_32/128               368 ns          368 ns      1903627
BM_hammingWeight_32/256               734 ns          734 ns       949554
BM_hammingWeight_32/512              1471 ns         1471 ns       477973
BM_hammingWeight_32/1024             2931 ns         2931 ns       238368
BM_hammingWeight_table/0             6.01 ns         6.01 ns    116482717
BM_hammingWeight_table/128           95.1 ns         95.1 ns      7309149
BM_hammingWeight_table/256            187 ns          187 ns      3736113
BM_hammingWeight_table/512            369 ns          369 ns      1891714
BM_hammingWeight_table/1024           736 ns          736 ns       956794
BM_hammingWeight_table0/0            5.67 ns         5.67 ns    123292159
BM_hammingWeight_table0/128          93.0 ns         93.0 ns      7653560
BM_hammingWeight_table0/256           182 ns          182 ns      3987253
BM_hammingWeight_table0/512           358 ns          358 ns      1969757
BM_hammingWeight_table0/1024          711 ns          711 ns       987284
*/