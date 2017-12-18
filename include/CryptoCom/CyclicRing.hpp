#pragma once

#include <cmath>
#include <CryptoCom/Eucledian.hpp>
#include <iostream>

namespace CryptoCom {

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


      CyclicRing< Traits >
      operator+( CyclicRing< Traits > const& other ) const {
        return CyclicRing< Traits > {
          ( ordinalIndex_ + other.ordinalIndex_ ) % Traits::Order
        };
      }

      CyclicRing< Traits > operator-() const {
        return CyclicRing< Traits > {
          Traits::Order - ordinalIndex_
        };
      }

      CyclicRing< Traits >
      operator-( CyclicRing< Traits > const& other ) const {
        return *this + (-other);
      }

      CyclicRing< Traits >
      operator*( CyclicRing< Traits > const& other ) const {
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

      CyclicRing< Traits >
      operator /( CyclicRing< Traits > const& other ) const {
        return *this * other.inverse();
      }
       
      friend std::ostream& operator<<(
          std::ostream&, CyclicRing< Traits > const& );
    };

} // CryptoCom
