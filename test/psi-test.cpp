#include <CryptoCom/ExponentialElGamal.hpp>
#include <CryptoCom/ObliviousEvaluation.hpp>

#include <iostream>
#include <random>
#include <set>


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
  using CoefficientType = int64_t;
  static constexpr PrimaryType Order = 2250635938/2;
  static constexpr PrimaryType Generator = 3;
  static constexpr PrimaryType AdditiveIdentity = 0;
  static constexpr PrimaryType MultiplicativeIdentity = 1;
};


int
main( int, char** )
{
  using Ring = CryptoCom::CyclicRing< RingTraits >;
  std::default_random_engine generator;
  std::uniform_int_distribution< int32_t > distribution( 1, RingTraits::Order );
//  auto rng = [&generator, &distribution]() -> Ring {
//    auto const x = distribution( generator );
//    return Ring{x};
//  };

  auto rng = []() {
//    auto const x = 12;
    auto x = Ring{12};
    return x;
  };

  using Encryption = CryptoCom::ExponentialElGamal< RingTraits >;
  auto keys = Encryption::KeyPairOf( rng );
  
  using ClientSet = CryptoCom::ObliviousEvaluation::ClientSet<
    Ring,
    int32_t,
    Encryption >;
  ClientSet client_set {
    std::get<0>( keys ), std::get<1>( keys ),
    { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 },
    rng };

  auto const polynomial = client_set.forServer();

  using ServerSet = CryptoCom::ObliviousEvaluation::ServerSet<
    Ring,
    int32_t,
    CryptoCom::ExponentialElGamal< RingTraits > >;
  ServerSet server_set { { 3, 6, 9, 12, 15, 18, 21 } };
  auto const evaluated = server_set.evaluate( polynomial, rng );

  auto const intersection = client_set.intersection(
      evaluated,
      std::get<1>( keys ) );
  std::cout << intersection << std::endl;

	return 0;
}
