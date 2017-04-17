#include "generator.h"
#include "gor_generator.h"

#include <range/v3/all.hpp>

template <class Generator>
Generator co_ints(int start, int end) {
  for (int i = start; i < end; ++i) {
    co_yield i;
  }
}

template <class F>
void cb_ints(int start, int end, F&& f) {
  for (int i = start; i < end; ++i) {
    f(i);
  }
}

extern void consume(int);

void bench_ints_generator_toby(int n) {
  RANGES_FOR(int i, co_ints<toby::generator<int>>(0, n)) { consume(i); }
}

void bench_ints_generator_gor(int n) {
  for (int i : co_ints<gor::generator<int>>(0, n)) {
    consume(i);
  }
}

#ifdef HAS_EXPERIMENTAL_GENERATOR

#include <experimental/generator>

void bench_ints_generator_exp(int n) {
  for (int i : co_ints<std::experimental::generator<int>>(0, n)) {
    consume(i);
  }
}

#endif

void bench_ints_generator_toby_atomic(int n) {
  RANGES_FOR(int i, co_ints<toby::generator<int, std::atomic<int>>>(0, n)) { consume(i); }
}

void bench_ints_handrolled(int n) {
  for (int i = 0; i < n; ++i) {
    consume(i);
  }
}

void bench_ints_ranges(int n) {
  for (auto i : ranges::view::ints(0, n)) {
    consume(i);
  }
}
