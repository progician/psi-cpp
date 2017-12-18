#pragma once

#include <cmath>
#include <stdexcept>
#include <tuple>

namespace CryptoCom {

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

} // CryptoCom
