#pragma once

#include <cmath>
#include <CryptoCom/Eucledian.hpp>
#include <iostream>
#include <type_traits>

namespace CryptoCom {

  template< typename RingTraits >
    class CyclicRing {
      typename RingTraits::PrimaryType ordinalIndex_;

    public:
      using Traits = RingTraits;

      constexpr CyclicRing( typename Traits::PrimaryType const ordinalIndex ) noexcept
        : ordinalIndex_(
            ( typename Traits::EscalationType( Traits::Order ) + ordinalIndex )
            % Traits::Order ) { } 

      constexpr CyclicRing( const CyclicRing<Traits>& other ) noexcept
        : ordinalIndex_( other.ordinalIndex_ ) {}

      constexpr CyclicRing() noexcept
        : ordinalIndex_( Traits::AdditiveIdentity ) {}

      static CyclicRing< Traits > constexpr Zero() {
        return CyclicRing< Traits > { Traits::AdditiveIdentity };
      }

      static CyclicRing< Traits > constexpr One() {
        return CyclicRing< Traits > { Traits::MultiplicativeIdentity };
      }

      static CyclicRing< Traits > constexpr Generator() {
        return CyclicRing< Traits > { Traits::Generator };
      }

      bool operator ==( CyclicRing const& other ) const {
        return ordinalIndex_ == other.ordinalIndex_;
      }

      bool operator !=( CyclicRing const& other ) const {
        return ordinalIndex_ != other.ordinalIndex_;
      }

      bool operator <( CyclicRing const other ) const {
        return ordinalIndex_ < other.ordinalIndex_;
      }


      CyclicRing< Traits >
      operator+( CyclicRing< Traits > const& other ) const {
        return CyclicRing< Traits > {
          ( ordinalIndex_ + other.ordinalIndex_ ) % Traits::Order
        };
      }

      CyclicRing< Traits >&
      operator+=( CyclicRing< Traits > const& other ) {
        ordinalIndex_ = ( ordinalIndex_ + other.ordinalIndex_ ) % Traits::Order;
        return *this;
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
        typename std::enable_if<
            std::is_signed< IntegralType >::value,
            CyclicRing< Traits >
        >::type pow( IntegralType exponent ) const {
          if ( exponent < 0 )
            return pow( std::abs( exponent ) ).inverse();

          if ( exponent == 0 )
            return CyclicRing< Traits >::One();

          if ( ( exponent % 2 ) == 1 )
            return *this * pow( exponent - 1 );

          auto const base { pow( exponent >> 1 ) };
          return base * base;
        }

      
      template< typename IntegralType >
        typename std::enable_if<
            !std::is_signed< IntegralType >::value,
            CyclicRing< Traits >
        >::type pow( IntegralType exponent ) const {
          if ( exponent == 0 )
            return CyclicRing< Traits >::One();

          if ( ( exponent % 2 ) == 1 )
            return *this * pow( exponent - 1 );

          auto const base { pow( exponent >> 1 ) };
          return base * base;
        }


      template< typename IntegralType >
        CyclicRing< Traits > operator^( IntegralType exponent ) const {
          return pow( exponent );
        }


      template< typename IntegralType >
        CyclicRing< Traits > operator%( IntegralType modulo ) const {
          return ordinalIndex_ % modulo;
        }

      template< typename IntegralType >
        CyclicRing< Traits > operator>>( IntegralType value ) const {
          return ordinalIndex_ >> value;
        }

      CyclicRing< Traits > inverse() const {
        return CyclicRing< Traits > {
          InverseModulo< typename Traits::PrimaryType, typename Traits::CoefficientType >( ordinalIndex_, Traits::Order ) };
      }

      CyclicRing< Traits >
      operator /( CyclicRing< Traits > const& other ) const {
        return *this * other.inverse();
      }
       
      friend std::ostream& operator<<(
          std::ostream&, CyclicRing< Traits > const& );
    };
} // CryptoCom


namespace std {
  template< typename Traits >
    struct is_signed< CryptoCom::CyclicRing< Traits > >
      : public std::false_type {};
} // std

