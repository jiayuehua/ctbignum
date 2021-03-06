//
// This file is part of
//
// CTBignum 	
//
// C++ Library for Compile-Time and Run-Time Multi-Precision and Modular Arithmetic
// 
//
// This file is distributed under the Apache License, Version 2.0. See the LICENSE
// file for details.
#include <benchmark/benchmark.h>
#include <random>

#include <ctbignum/bigint.hpp>
#include <ctbignum/montgomery.hpp>
#include <ctbignum/decimal_literals.hpp>

#include <libff/algebra/fields/fp.hpp>
#include <libff/algebra/fields/bigint.hpp>

using namespace cbn::literals;


template <size_t Len> static void montmul_cbn(benchmark::State &state) {

  using namespace cbn;

  size_t total_sz = 2 * Len * 1000;

  std::vector<uint64_t> data(total_sz);
  std::default_random_engine generator;
  std::uniform_int_distribution<uint64_t> distribution(0);
  for (auto &limb : data)
    limb = distribution(generator);

  size_t i = 0;
  auto base_ptr = data.data();

  for (auto _ : state) {

    auto x = reinterpret_cast<big_int<Len> *>(base_ptr + i);
    auto y = reinterpret_cast<big_int<Len> *>(base_ptr + i + Len);
    auto j = cbn::montgomery_mul(
        *x, *y,
        14474011154664524427946373126085988481658748083205070504932198000989141205031_Z);
    benchmark::DoNotOptimize(j);

    i += 2 * Len;
    if (i == total_sz)
      i = 0;
  }
}

static auto mymodulus = libff::bigint<4>("14474011154664524427946373126085988481658748083205070504932198000989141205031");

template <size_t Len> static void montmul_libff_t(benchmark::State &state) {

  size_t total_sz = 2 * Len * 1000;

  std::vector<uint64_t> data(total_sz);
  std::default_random_engine generator;
  std::uniform_int_distribution<uint64_t> distribution(0);
  for (auto &limb : data)
    limb = distribution(generator);

  size_t i = 0;
  auto base_ptr = data.data();

  using namespace libff;

  using GF253 = Fp_model<4L, mymodulus>;
  GF253::inv = 10405855631323336809UL;

  for (auto _ : state) {

    auto x = reinterpret_cast<GF253 *>(base_ptr + i);
    auto y = reinterpret_cast<GF253 *>(base_ptr + i + Len);

    auto z = *x * *y;

    benchmark::DoNotOptimize(z);

    i += 2 * Len;
    if (i == total_sz)
      i = 0;
  }
}

BENCHMARK_TEMPLATE(montmul_cbn,4);
BENCHMARK_TEMPLATE(montmul_libff_t,4);
BENCHMARK_MAIN();
