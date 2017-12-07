#include <catch/catch.hpp>

struct ExampleRing {
  static constexpr int64_t Order = 0x800000;
  static constexpr int64_t Generator = 2;
  static constexpr int64_t AdditiveIdentity = 0;
};


template< typename Traits >
  class Elem {
    int64_t const ordinalIndex_;

  public:
    constexpr Elem( const int64_t ordinalIndex )
      : ordinalIndex_( ordinalIndex < Traits::Order
          ? ordinalIndex
          : throw std::runtime_error( "ordinalIndex exceeds ring's order" ) ) { } 

    constexpr Elem()
      : ordinalIndex_( Traits::AdditiveIdentity ) {}

    bool operator ==( Elem const& other ) const {
      return ordinalIndex_ == other.ordinalIndex_;
    }


    Elem< Traits > operator+( Elem< Traits > const& other ) {
      return Elem< Traits > {
        ( ordinalIndex_ + other.ordinalIndex_ ) % Traits::Order
      };
    }

    Elem< Traits > operator-() const {
      return Elem< Traits > {
        Traits::Order - ordinalIndex_
      };
    }
  };



TEST_CASE( "In cyclic groups" ) {
  SECTION( "uninitialized element is identity for adding" ) {
    Elem< ExampleRing > a { 3 };
    Elem< ExampleRing > b;
    REQUIRE( ( a + b ) == a );
  }

  SECTION( "adding element and it's additive inverse is additive identity" ) {
    Elem< ExampleRing > a { 3 };
    auto const b = -a;
    auto const zero = ExampleRing::AdditiveIdentity;
    REQUIRE( a + b == zero );
  } 
}
