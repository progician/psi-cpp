#include <catch/catch.hpp>


struct SimpleCyclicGroupTraits {
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
  };



TEST_CASE( "In cyclic groups" ) {
  SECTION( "uninitialized element is identity for adding" ) {
    Elem< SimpleCyclicGroupTraits > a { 3 };
    Elem< SimpleCyclicGroupTraits > b;
    REQUIRE( ( a + b ) == a );
  }
}
