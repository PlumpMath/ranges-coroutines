#define SPDLOG_FMT_EXTERNAL
#include "generator.h"

//#include <doctest/doctest.h>
#include <range/v3/all.hpp>
#include "doctest.h"

#include <memory>

using toby::generator;

generator<int> upto(int n) {
  for (int i = 0; i < n; ++i) co_yield i;
}

generator<std::unique_ptr<int>> move_only() {
  co_yield std::make_unique<int>(7);
  co_yield std::make_unique<int>(42);
}

TEST_CASE("empty sequence") {
  auto g = upto(0);
  CHECK(g.begin() == g.end());
}

TEST_CASE("one element") {
  auto g = upto(1);
  auto b = g.begin();
  CHECK(b != g.end());
  CHECK(*b == 0);
  CHECK(*b == 0);
  auto&& v = *b;
  CHECK(v == 0);
  ++b;
  CHECK(b == g.end());
  CHECK(v == 0);
}

TEST_CASE("three elements") {
  auto g = upto(3);
  auto i = g.begin();
  REQUIRE(i != g.end());
  auto v = *i;
  SUBCASE("first element is zero") { CHECK(v == 0); }
  ++i;
  REQUIRE(i != g.end());
  SUBCASE("incrementing the iterator doesn't change the previous value") {
    CHECK(v == 0);
  }
  auto&& v2 = *i;
  SUBCASE("second element is one") { CHECK(v2 == 1); }
  i++;
  i++;
  SUBCASE("post-increment to end") { CHECK(i == g.end()); }
}

TEST_CASE("move-only type") {
  auto g = move_only();
  auto i = g.begin();
  CHECK(**i == 7);
  auto v = std::move(*i);
  CHECK(*v == 7);
  i++;
  REQUIRE(*i);
  CHECK(**i == 42);
  auto v2 = std::move(*i);
  CHECK(*v2 == 42);
  i++;
  CHECK(i == g.end());
}

TEST_CASE("move iterator") {
// these tests fail because std::move_iterator::operator++(int) is WRONG for input
// iterators (see http://open-std.org/JTC1/SC22/WG21/docs/papers/2017/p0541r0.html)
#if move_iterator_is_fixed
  auto g = move_only();
  auto i = std::make_move_iterator(g.begin());
  CHECK(**i == 7);
  auto v = *i++;
  CHECK(*v == 7);
  REQUIRE(*i);
  CHECK(**i == 42);
  auto v2 = *i;
  CHECK(*v2 == 42);
  i++;
  CHECK(i == std::make_move_iterator(g.end()));
#endif
}

generator<int> infinite() {
  for (int i = 0;; ++i) {
    co_yield i;
  }
}

TEST_CASE("infinite generator") {
  auto g = infinite();
  SUBCASE("first coouple of elements") {
    auto i = g.begin();
    CHECK(*i == 0);
    ++i;
    CHECK(*i == 1);
    i++;
  }
  SUBCASE("take 5") {
    CONCEPT_ASSERT(ranges::v3::Destructible<decltype(g.begin())>::value);
    CONCEPT_ASSERT(ranges::v3::Iterator<decltype(g.begin())>::value);
    CONCEPT_ASSERT(ranges::v3::Readable<decltype(g.begin())>::value);
    CONCEPT_ASSERT(ranges::v3::InputIterator<decltype(g.begin())>::value);
    CONCEPT_ASSERT(ranges::v3::Regular<decltype(g.end())>::value);
#if !defined(RANGE_V3_VERSION) || RANGE_V3_VERSION < 200
    CONCEPT_ASSERT(
        ranges::v3::CommonReference<decltype(g.begin()), decltype(g.end())>::value);
    CONCEPT_ASSERT(
        ranges::v3::EqualityComparable<decltype(g.begin()), decltype(g.end())>::value);
#endif
    CONCEPT_ASSERT(ranges::v3::Range<decltype(g)>::value);
    CONCEPT_ASSERT(ranges::v3::InputRange<decltype(g)>::value);
    auto a             = ranges::view::take(5) | ranges::to_vector;
    auto b             = g | ranges::view::take(5);
    std::vector<int> v = b;
    CHECK(v == std::vector<int>({0, 1, 2, 3, 4}));
  }
}
