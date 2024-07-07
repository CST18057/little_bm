#include <benchmark/benchmark.h>
#include <cstdint>
// taskset -c 0  ./hamming_weight
#define BM(x) BENCHMARK(BM_##x)

#include <algorithm>
#include <random>
#include <vector>
#include <cstdint>
#include <immintrin.h>
#include <cstring>
#include <cstdio>
#include <array>
const int M = (1 << 14);
uint16_t bins[64] = {0};
uint8_t data[M] = {0};

void prepare() {
      memset(bins, 0, 64 * sizeof(uint16_t));
    // Fill data with some values (for example purposes)
    for (int i = 0; i < M; i++) {
        data[i] = static_cast<uint8_t>(i % 64);
    }
}

// Function to compute the histogram of the lower 6 bits of each data element
void histgram_base(uint16_t *bins, uint8_t *data) {
    // Iterate over all data elements
    for (int i = 0; i < M; i++) {
        // Mask the lower 6 bits of the data element
        uint8_t x = data[i] & 0x3f;
        // Increment the corresponding bin
        bins[x]++;
    }
}

void histgram_avx(uint16_t *bins, uint8_t *data) {

    __m256i mask = _mm256_set1_epi8(0x3F);

    for (int i = 0; i < M; i += 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        chunk = _mm256_and_si256(chunk, mask);

        uint8_t tmp[32];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp), chunk);

        for (int j = 0; j < 32; ++j) {
            bins[tmp[j]]++;
        }
    }
}

using Bucket = std::array<uint16_t, 64>;

// 统计 data 中 0~63 的频数，不考虑高 2 位
std::array<Bucket, 8> histgram_mwish(uint16_t *bins, uint8_t *data) {
    std::array<Bucket, 8> temp_bins;
    for (int i = 0; i < M / 8; i++) {
        for (int idx = 0; idx < 8; ++idx) {
            uint8_t x = data[i * 8 + idx] % 64;
            temp_bins[idx][x]++;
        }
    }
    return temp_bins;
}


void BM_histgram_base(benchmark::State& state) {
  prepare();
  for (auto _ : state) {
    histgram_base(bins, data);
  }
}


void BM_histgram_avx(benchmark::State& state) {
  prepare();
  for (auto _ : state) {
    histgram_avx(bins, data);
  }
}

// 这个有点不太公平
void BM_histgram_mwish(benchmark::State& state) {
  prepare();
  for (auto _ : state) {
    benchmark::DoNotOptimize(histgram_mwish(bins, data));
  }
}

BM(histgram_mwish)->Iterations(10000000);

BM(histgram_base)->Iterations(10000000);

BM(histgram_avx)->Iterations(10000000);

BENCHMARK_MAIN();

