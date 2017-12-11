#include <catch/catch.hpp>
#include <cmath>

struct ExampleRing {
  using PrimaryType = int32_t;
  using EscalationType = int64_t;
  static constexpr PrimaryType Order = 0x800000;
  static constexpr PrimaryType Generator = 2;
  static constexpr PrimaryType AdditiveIdentity = 0;
  static constexpr PrimaryType MultiplicativeIdentity = 1;
};


template< typename Traits >
  class Elem {
    typename Traits::PrimaryType const ordinalIndex_;

  public:
    constexpr Elem( typename Traits::PrimaryType const ordinalIndex )
      : ordinalIndex_( ordinalIndex < Traits::Order
          ? ordinalIndex
          : throw std::runtime_error( "ordinalIndex exceeds ring's order" ) ) { } 

    constexpr Elem()
      : ordinalIndex_( Traits::AdditiveIdentity ) {}

    static Elem< Traits > constexpr zero() {
      return Elem< Traits > { Traits::AdditiveIdentity };
    }

    static Elem< Traits > constexpr one() {
      return Elem< Traits > { Traits::MultiplicativeIdentity };
    }

    bool operator ==( Elem const& other ) const {
      return ordinalIndex_ == other.ordinalIndex_;
    }


    Elem< Traits > operator+( Elem< Traits > const& other ) const {
      return Elem< Traits > {
        ( ordinalIndex_ + other.ordinalIndex_ ) % Traits::Order
      };
    }

    Elem< Traits > operator-() const {
      return Elem< Traits > {
        Traits::Order - ordinalIndex_
      };
    }

    Elem< Traits >  operator-( Elem< Traits > const& other ) const {
      return *this + (-other);
    }

    Elem< Traits > operator*( Elem< Traits > const& other ) const {
      typename Traits::EscalationType escalatedResults =
        ordinalIndex_ * other.ordinalIndex_;
      return static_cast< typename Traits::PrimaryType >(
          escalatedResults % Traits::Order );
    }

    template< typename IntegralType >
      Elem< Traits > pow( IntegralType exponent ) const {
        if ( exponent == 0 )
          return Elem< Traits >::one();

        if ( ( exponent % 2 ) == 1 )
          return *this * pow( exponent - 1 );

        auto const base { pow( exponent >> 1 ) };
        return base * base;
      }
     
    friend std::ostream& operator<<( std::ostream&, Elem< Traits > const& );
  };


std::ostream&
operator<<( std::ostream& outputStream, Elem< ExampleRing > const& v ) {
  outputStream << v.ordinalIndex_; 
  return outputStream;
}



TEST_CASE( "In cyclic rings" ) {
  Elem< ExampleRing > constexpr zero;

  SECTION( "uninitialized element is additive identity" ) {
    Elem< ExampleRing > constexpr a { 3 };
    REQUIRE( ( a + zero ) == a );
  }

  SECTION( "adding element and its additive inverse is additive identity" ) {
    Elem< ExampleRing > constexpr a { 3 };
    auto const b = -a;
    REQUIRE( a + b == zero );
  } 

  SECTION( "subtraction is adding additive inverse" ) {
    Elem< ExampleRing > constexpr a { 3 };
    REQUIRE( a - a == zero );
  }

  SECTION( "multiplying with multiplicative identity" ) {
    auto constexpr one = Elem< ExampleRing >::one();
    Elem< ExampleRing > constexpr a { 3 };

    REQUIRE( a * one == a );
  }

  SECTION( "power is repeated modulo-multiplication" ) {
    Elem< ExampleRing > constexpr a { 3 };
    REQUIRE( a.pow( 2 ) == 9 );
  }
}
