#include "private-intersection.hpp"

#include <iostream>
#include <set>
#include <tuple>
#include <vector>


std::ostream&
operator <<( std::ostream& ostr, std::set< int64_t > const& v )
{
  for ( const int64_t n: v ) {
		ostr << n << " ";
	}

	return ostr;
}


int
main( int, char** )
{
  std::vector< int64_t > const clientSet = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 };
  std::vector< int64_t > const serverSet = { 3, 6, 9, 12, 15, 18, 21 };
  fg::Group cryptoGroup( 2250635938ull, 3 );

  auto const keys = ElGamal::keygen( cryptoGroup );

  auto const fromClient = PrivateIntersection::prepareLocalSet( clientSet.begin(), clientSet.end(), std::get<1>( keys ) );
  auto const fromServer = PrivateIntersection::obliviousEvaluation( serverSet.begin(), serverSet.end(), fromClient );
  auto const intersection = PrivateIntersection::extractIntersection< int64_t, std::vector< int64_t >::const_iterator >( clientSet.begin(), clientSet.end(), fromServer, std::get<0>( keys ) );

  std::cout << intersection << std::endl;

	return 0;
}
