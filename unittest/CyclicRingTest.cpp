#include <catch/catch.hpp>

#include <CryptoCom/CyclicRing.hpp>

struct TestRingTraits {
  using PrimaryType = int32_t;
  using EscalationType = int64_t;
  using CoefficientType = int32_t;

  static constexpr PrimaryType Order{37};
  static constexpr PrimaryType Generator{2};
  static constexpr PrimaryType AdditiveIdentity{0};
  static constexpr PrimaryType MultiplicativeIdentity{1};
};
using TestRing = CryptoCom::CyclicRing<TestRingTraits>;


namespace CryptoCom {
  std::ostream&
  operator <<(std::ostream& ostr, TestRing const& e) {
    ostr << e.ordinalIndex_;
    return ostr;
  }

} // CryptoCom


TEST_CASE("In cyclic rings") {
  constexpr TestRing Zero;
  constexpr auto one = TestRing::One();

  SECTION("uninitialized element is additive identity") {
    constexpr TestRing a {3};
    REQUIRE((a + Zero) == a);
  }

  SECTION("adding element and its additive inverse is additive identity") {
    constexpr TestRing a{3};
    auto const b = -a;
    REQUIRE(a + b == Zero);
  } 

  SECTION("subtraction is adding additive inverse") {
    constexpr TestRing a{3};
    REQUIRE(a - a == Zero);
  }

  SECTION("multiplying with multiplicative identity") {
    constexpr TestRing a{3};

    REQUIRE(a * one == a);
  }

  SECTION("power is repeated modulo-multiplication") {
    constexpr TestRing a{3};
    REQUIRE(a.pow(2) == 9);
  }

  SECTION("multiplication with multiplicative inverse is (multiplicative) identity") {
    constexpr TestRing  a{3};
    REQUIRE(a * a.inverse() == one);
  }

  SECTION("division is multiplication with multiplicative inverse") {
    constexpr TestRing twenty_seven{ 27 };
    constexpr TestRing three{3};
    constexpr TestRing nine{9};
    REQUIRE(twenty_seven / three == nine);
    REQUIRE(twenty_seven / three == twenty_seven * three.inverse());
  }

  SECTION("negative exponent is multiplied with multiplicative inverse") {
    constexpr TestRing a {3};
    REQUIRE(a.pow(-1) == a.inverse());
    REQUIRE(a.pow(-2) == TestRing{9}.inverse());
  }

  SECTION("operator ^ is power") {
    constexpr TestRing three{3};
    REQUIRE((three ^ 2) == 9);
  }
}
