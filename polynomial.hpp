#pragma once

#include <boost/assert.hpp>
#include "finite-group.hpp"
#include <iterator>
#include <vector>

namespace poly {

  template< typename CoefficientType, typename VariableType,
            typename PolynomialIt >
    CoefficientType
    eval( VariableType const& x, PolynomialIt first, PolynomialIt last )
    {
      BOOST_ASSERT( first != last );

      if ( std::next( first ) == last )
        return *first;

      return *first + eval<CoefficientType, VariableType, PolynomialIt>( x, std::next( first ), last ) * x;
    }


  template< typename CoefficientType >
    std::vector< CoefficientType >
    mul( std::vector< CoefficientType > const& a,
         std::vector< CoefficientType > const& b,
         CoefficientType zero )
    {
      std::vector< CoefficientType > result( a.size() + b.size() - 1, zero );
      for ( size_t aIndex = 0; aIndex < a.size(); ++aIndex ) {
        for ( size_t bIndex = 0; bIndex < b.size(); ++bIndex ) {
          result[ aIndex + bIndex ] += a[ aIndex ] * b[ bIndex ];
        }
      }

      return result;
    }


  template< typename CoefficientType,
            typename RootIt >
    std::vector< CoefficientType >
    fromRoots( RootIt first, RootIt last, CoefficientType minusOne, CoefficientType plusOne )
    {
      const CoefficientType zero = minusOne + plusOne;
      std::vector< CoefficientType > polynomial = { minusOne * (*first), plusOne };
      for ( RootIt current = std::next( first ); current != last; ++current ) {
        auto const root = *current;
        polynomial = mul( polynomial, { minusOne * root, plusOne }, zero );
      }

      return polynomial;
    }
} // End of namespace 'poly'
