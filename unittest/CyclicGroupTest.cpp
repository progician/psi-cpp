#include <catch/catch.hpp>

#include <CryptoCom/CyclicRing.hpp>

struct TestRingTraits {
  using PrimaryType = int32_t;
  using EscalationType = int64_t;
  static constexpr PrimaryType Order = 0x800000;
  static constexpr PrimaryType Generator = 2;
  static constexpr PrimaryType AdditiveIdentity = 0;
  static constexpr PrimaryType MultiplicativeIdentity = 1;
};


namespace CryptoCom {
  std::ostream&
  operator <<( std::ostream& ostr, CyclicRing< TestRingTraits > const& e ) {
    ostr << e.ordinalIndex_;
    return ostr;
  }

} // CryptoCom


TEST_CASE( "In cyclic rings" ) {
  CryptoCom::CyclicRing< TestRingTraits > constexpr zero;
  auto constexpr one = CryptoCom::CyclicRing< TestRingTraits >::one();

  SECTION( "uninitialized element is additive identity" ) {
    CryptoCom::CyclicRing< TestRingTraits > constexpr a { 3 };
    REQUIRE( ( a + zero ) == a );
  }

  SECTION( "adding element and its additive inverse is additive identity" ) {
    CryptoCom::CyclicRing< TestRingTraits > constexpr a { 3 };
    auto const b = -a;
    REQUIRE( a + b == zero );
  } 

  SECTION( "subtraction is adding additive inverse" ) {
    CryptoCom::CyclicRing< TestRingTraits > constexpr a { 3 };
    REQUIRE( a - a == zero );
  }

  SECTION( "multiplying with multiplicative identity" ) {
    CryptoCom::CyclicRing< TestRingTraits > constexpr a { 3 };

    REQUIRE( a * one == a );
  }

  SECTION( "power is repeated modulo-multiplication" ) {
    CryptoCom::CyclicRing< TestRingTraits > constexpr a { 3 };
    REQUIRE( a.pow( 2 ) == 9 );
  }

  SECTION( "multiplication with multiplicative inverse is (multiplicative) identity" ) {
    CryptoCom::CyclicRing< TestRingTraits > constexpr a { 3 };
    REQUIRE( a * a.inverse() == one );
  }

  SECTION( "division is multiplication with multiplicative inverse" ) {
    CryptoCom::CyclicRing< TestRingTraits > constexpr twenty_seven { 27 };
    CryptoCom::CyclicRing< TestRingTraits > constexpr three { 3 };
    CryptoCom::CyclicRing< TestRingTraits > constexpr nine { 9 };
    REQUIRE( twenty_seven / three == nine );
    REQUIRE( twenty_seven / three == twenty_seven * three.inverse() );
  }

  SECTION( "negative exponent is multiplied with multiplicative inverse" ) {
    CryptoCom::CyclicRing< TestRingTraits > constexpr a { 3 };
    REQUIRE( a.pow( -1 ) == a.inverse() );
    REQUIRE( a.pow( -2 ) == CryptoCom::CyclicRing< TestRingTraits >{ 9 }.inverse() );
  }
}
