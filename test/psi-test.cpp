#include <CryptoCom/private-intersection.hpp>

#include <iostream>
#include <random>
#include <set>
#include <tuple>
#include <vector>


std::ostream&
operator <<( std::ostream& ostr, std::set< int32_t > const& v )
{
  for ( auto const n: v ) {
		ostr << n << " ";
	}

	return ostr;
}


struct RingTraits {
  using PrimaryType = int32_t;
  using EscalationType = int64_t;
  static constexpr PrimaryType Order = 0x800000;
  static constexpr PrimaryType Generator = 3;
  static constexpr PrimaryType AdditiveIdentity = 0;
  static constexpr PrimaryType MultiplicativeIdentity = 1;
};


CryptoCom::CyclicRing< RingTraits > rng() {
  return {};
}


int
main( int, char** )
{
  using ElGamal = CryptoCom::ElGamal< RingTraits >;
  using PSI = CryptoCom::PrivateIntersection< RingTraits >;
  std::vector< int32_t > const clientSet = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 };
  std::vector< int32_t > const serverSet = { 3, 6, 9, 12, 15, 18, 21 };

  auto const keys = ElGamal::newKeyPair( rng );

  auto const fromClient = PSI::encryptedPolynomial( clientSet.begin(), clientSet.end(), std::get<1>( keys ), rng );
  auto const fromServer = PSI::obliviousEvaluation( serverSet.begin(), serverSet.end(), fromClient, rng );
  auto const intersection = PSI::extractIntersection< int32_t, std::vector< int32_t >::const_iterator >( clientSet.begin(), clientSet.end(), fromServer, std::get<0>( keys ) );

  std::cout << intersection << std::endl;

	return 0;
}
