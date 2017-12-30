#include <CryptoCom/polynomial.hpp>
#include <catch/catch.hpp>

namespace CryptoCom {
  std::ostream& operator<<( std::ostream& ostr, Polynomial< int > const& p ) {
    ostr << "{ ";
    for ( size_t idx = 0; idx < p.size() - 1; ++idx ) {
      ostr << p[ idx ] << ", ";
    } 
    ostr << p[ p.size() - 1 ] << " }";
    return ostr;
  }
} // CryptoCom


TEST_CASE( "Generic polynomials" ) {

  SECTION( "constructed by specifying coefficients" ) {
    CryptoCom::Polynomial< int > const polynomial {
      2, 1 };

    SECTION( "evaluated by a variable gives a specific solution" ) {
      REQUIRE( polynomial( 4 ) == 6 );
    }

    SECTION( "multiplying polynomials is composition" ) {
      CryptoCom::Polynomial< int > const expected { 4, 4, 1 };
      REQUIRE( polynomial * polynomial == expected );
    }
  }


  SECTION( "constructing from roots will evaluate to 0 for all roots" ) {
    int roots[] = { 4, 9 };
    auto const p = CryptoCom::Polynomial< int >::fromRoots(
        std::begin( roots ), std::end( roots ) );

    CHECK( p( 4 ) == 0 );
    CHECK( p( 9 ) == 0 );

    CryptoCom::Polynomial< int > expected { 36, -13, 1 };
    REQUIRE( p == expected ); 
  }
}
