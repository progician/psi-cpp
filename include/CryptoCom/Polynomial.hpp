#pragma once

#include <initializer_list>
#include <iterator>
#include <vector>

namespace CryptoCom {

  template< typename CoefficientType >
    class Polynomial {
      std::vector< CoefficientType  > coefficients_;
    public:

      Polynomial() = default;
      Polynomial( std::initializer_list< CoefficientType > l )
        : coefficients_( l ) {}
      Polynomial( std::vector< CoefficientType >&& coeffs )
        : coefficients_( coeffs ) {}

      template< typename VariableType >
        CoefficientType operator()( VariableType const& x ) const {
          CoefficientType result { coefficients_[ coefficients_.size() - 1 ] };
          for ( size_t idx = 1; idx < coefficients_.size(); idx++ ) {
            result = result * x + coefficients_[ coefficients_.size() - 1 - idx ];
          }
          return result;
        }


      Polynomial< CoefficientType >
      operator *( Polynomial< CoefficientType > const& other ) const {
        Polynomial res;
        res.coefficients_.resize(
            coefficients_.size() + other.coefficients_.size() - 1,
            CoefficientType{} );
        for ( size_t i = 0; i < coefficients_.size(); i++ ) {
          for ( size_t j = 0; j < other.coefficients_.size(); j++ ) {
            res[ i + j ] += coefficients_[ i ] * other.coefficients_[ j ];
          }
        }

        return res;
      }


      CoefficientType&
      operator[]( size_t idx ) { return coefficients_[ idx ]; }
      
      CoefficientType
      operator[]( size_t idx ) const { return coefficients_[ idx ]; }

      size_t size() const { return coefficients_.size(); }


      typename std::vector< CoefficientType >::iterator begin()
      { return coefficients_.begin(); }

      typename std::vector< CoefficientType >::const_iterator cbegin() const
      { return coefficients_.cbegin(); }

      typename std::vector< CoefficientType >::iterator end()
      { return coefficients_.end(); }

      typename std::vector< CoefficientType >::const_iterator cend() const
      { return coefficients_.cend(); }

      bool operator==( Polynomial< CoefficientType > const& other ) const {
        if ( coefficients_.size() != other.coefficients_.size() )
          return false;
        return std::equal(
            coefficients_.begin(), coefficients_.end(),
            other.coefficients_.begin() );
      }


      template< typename RootIt >
        static Polynomial< CoefficientType >
        fromRoots(
            RootIt first, RootIt last,
            CoefficientType const minusOne = -1,
            CoefficientType const plusOne = 1 ) {
          Polynomial< CoefficientType > polynomial { minusOne * (*first), plusOne };
          for ( auto current = std::next( first ); current != last; ++current ) {
            auto const root = *current;
            polynomial = polynomial * Polynomial< CoefficientType >{ minusOne * root, plusOne };
          }

          return polynomial;
        }
      friend std::ostream& operator<<( std::ostream&, Polynomial< CoefficientType > const& );
    };

} // End of namespace 'poly'
