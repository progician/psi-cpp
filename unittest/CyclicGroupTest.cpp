#include <catch/catch.hpp>
#include <cmath>

struct TestRingTraits {
  using PrimaryType = int32_t;
  using EscalationType = int64_t;
  static constexpr PrimaryType Order = 0x800000;
  static constexpr PrimaryType Generator = 2;
  static constexpr PrimaryType AdditiveIdentity = 0;
  static constexpr PrimaryType MultiplicativeIdentity = 1;
};


namespace {
  template< typename T >
    std::tuple< T, T, T> ExtendedGCD( T a, T b ) {
      if ( a == 0 )
        return std::make_tuple( b, 0, 1 );
      
      T g, x, y;
      std::tie( g, x, y ) = ExtendedGCD( b % a, a );
      return std::make_tuple( g, y - ( ( b / a ) * x ), x );
    }

  template< typename T >
    T InverseModulo( T const a, T const b ) {
      T g, x;
      std::tie( g, x, std::ignore ) = ExtendedGCD( a, b );

      if ( g == 1 ) {
        if ( x < 0 )
          return b + x;

        return std::abs( x ) % b;
      }

      throw std::invalid_argument( "relative primes have no inverse modulo" );
    }
}


template< typename Traits >
  class CyclicRing {
    typename Traits::PrimaryType const ordinalIndex_;

  public:
    constexpr CyclicRing( typename Traits::PrimaryType const ordinalIndex )
      : ordinalIndex_( ordinalIndex < Traits::Order
          ? ordinalIndex
          : throw std::runtime_error( "ordinalIndex exceeds ring's order" ) ) { } 

    constexpr CyclicRing()
      : ordinalIndex_( Traits::AdditiveIdentity ) {}

    static CyclicRing< Traits > constexpr zero() {
      return CyclicRing< Traits > { Traits::AdditiveIdentity };
    }

    static CyclicRing< Traits > constexpr one() {
      return CyclicRing< Traits > { Traits::MultiplicativeIdentity };
    }

    bool operator ==( CyclicRing const& other ) const {
      return ordinalIndex_ == other.ordinalIndex_;
    }


    CyclicRing< Traits > operator+( CyclicRing< Traits > const& other ) const {
      return CyclicRing< Traits > {
        ( ordinalIndex_ + other.ordinalIndex_ ) % Traits::Order
      };
    }

    CyclicRing< Traits > operator-() const {
      return CyclicRing< Traits > {
        Traits::Order - ordinalIndex_
      };
    }

    CyclicRing< Traits >  operator-( CyclicRing< Traits > const& other ) const {
      return *this + (-other);
    }

    CyclicRing< Traits > operator*( CyclicRing< Traits > const& other ) const {
      typename Traits::EscalationType escalatedResults =
        ordinalIndex_ * other.ordinalIndex_;
      return static_cast< typename Traits::PrimaryType >(
          escalatedResults % Traits::Order );
    }

    template< typename IntegralType >
      CyclicRing< Traits > pow( IntegralType exponent ) const {
        if ( exponent < 0 )
          return pow( std::abs( exponent ) ).inverse();

        if ( exponent == 0 )
          return CyclicRing< Traits >::one();

        if ( ( exponent % 2 ) == 1 )
          return *this * pow( exponent - 1 );

        auto const base { pow( exponent >> 1 ) };
        return base * base;
      }


    CyclicRing< Traits > inverse() const {
      return CyclicRing< Traits > {
        InverseModulo( ordinalIndex_, Traits::Order ) };
    }

    CyclicRing< Traits > operator /( CyclicRing< Traits > const& other ) const {
      return *this * other.inverse();
    }
     
    friend std::ostream& operator<<( std::ostream&, CyclicRing< Traits > const& );
  };


std::ostream&
operator<<( std::ostream& outputStream, CyclicRing< TestRingTraits > const& v ) {
  outputStream << v.ordinalIndex_; 
  return outputStream;
}



TEST_CASE( "In cyclic rings" ) {
  CyclicRing< TestRingTraits > constexpr zero;
  auto constexpr one = CyclicRing< TestRingTraits >::one();

  SECTION( "uninitialized element is additive identity" ) {
    CyclicRing< TestRingTraits > constexpr a { 3 };
    REQUIRE( ( a + zero ) == a );
  }

  SECTION( "adding element and its additive inverse is additive identity" ) {
    CyclicRing< TestRingTraits > constexpr a { 3 };
    auto const b = -a;
    REQUIRE( a + b == zero );
  } 

  SECTION( "subtraction is adding additive inverse" ) {
    CyclicRing< TestRingTraits > constexpr a { 3 };
    REQUIRE( a - a == zero );
  }

  SECTION( "multiplying with multiplicative identity" ) {
    CyclicRing< TestRingTraits > constexpr a { 3 };

    REQUIRE( a * one == a );
  }

  SECTION( "power is repeated modulo-multiplication" ) {
    CyclicRing< TestRingTraits > constexpr a { 3 };
    REQUIRE( a.pow( 2 ) == 9 );
  }

  SECTION( "multiplication with multiplicative inverse is (multiplicative) identity" ) {
    CyclicRing< TestRingTraits > constexpr a { 3 };
    REQUIRE( a * a.inverse() == one );
  }

  SECTION( "division is multiplication with multiplicative inverse" ) {
    CyclicRing< TestRingTraits > constexpr twenty_seven { 27 };
    CyclicRing< TestRingTraits > constexpr three { 3 };
    CyclicRing< TestRingTraits > constexpr nine { 9 };
    REQUIRE( twenty_seven / three == nine );
    REQUIRE( twenty_seven / three == twenty_seven * three.inverse() );
  }

  SECTION( "negative exponent is multiplied with multiplicative inverse" ) {
    CyclicRing< TestRingTraits > constexpr a { 3 };
    REQUIRE( a.pow( -1 ) == a.inverse() );
    REQUIRE( a.pow( -2 ) == CyclicRing< TestRingTraits >{ 9 }.inverse() );
  }
}
