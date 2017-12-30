#pragma once

#include <cmath>
#include <stdexcept>
#include <tuple>

namespace CryptoCom {

  template< typename T, typename CoefficientType >
    std::tuple< T, CoefficientType, CoefficientType > ExtendedGCD( T a, T b ) {
      if ( a == 0 )
        return std::make_tuple( b, 0, 1 );
      
      T g;
      CoefficientType x, y;
      std::tie( g, x, y ) = ExtendedGCD< T, CoefficientType >( b % a, a );
      auto const s = y - ( CoefficientType( b ) / CoefficientType( a ) ) * x;
      return std::make_tuple( g, s, x );
    }


  template< typename T, typename CoefficientType >
    T InverseModulo( T const a, T const b ) {
      T g;
      CoefficientType x;
      std::tie( g, x, std::ignore ) = ExtendedGCD< T, CoefficientType >( a, b );

      if ( g == 1 ) {
        if ( x < 0 )
          return T( b + x );

        return T( std::abs( x ) ) % b;
      }

      throw std::invalid_argument( "relative primes have no inverse modulo" );
    }

} // CryptoCom
