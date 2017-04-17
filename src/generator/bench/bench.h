#ifndef BENCH_H
#define BENCH_H

void bench_ints_generator_toby(int n);
void bench_ints_generator_gor(int n);
#ifdef HAS_EXPERIMENTAL_GENERATOR
void bench_ints_generator_exp(int n);
#endif
void bench_ints_generator_toby_atomic(int n);
void bench_ints_handrolled(int n);
void bench_ints_ranges(int n);

#endif // BENCH_H
