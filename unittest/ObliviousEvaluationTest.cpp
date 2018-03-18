#include <CryptoCom/ObliviousEvaluation.hpp>
#include <catch/catch.hpp>


namespace {
  struct RingTraits {
    using PrimaryType = int32_t;
    using EscalationType = int64_t;
    using CoefficientType = int64_t;

    static constexpr PrimaryType Order = 1483;
    static constexpr PrimaryType Generator = 2;
    static constexpr PrimaryType AdditiveIdentity = 0;
    static constexpr PrimaryType MultiplicativeIdentity = 1;
  };
}


namespace CryptoCom {

  std::ostream&
  operator <<( std::ostream& ostr, CyclicRing< RingTraits > const& e ) {
    ostr << e.ordinalIndex_;
    return ostr;
  }


  std::ostream&
  operator <<( std::ostream& ostr, ExponentialElGamal< RingTraits >::Cipher const& c ) {
    ostr << "("
         << std::get< 0 >( c.components )
         << ", "
         << std::get< 1 >( c.components )
         << ")";
    return ostr;
  }

} // CryptoCom



TEST_CASE( "Private set intersection using oblivious polynomial evaluation" ) {
  using namespace CryptoCom::ObliviousEvaluation;
  using TestClientSet = ClientSet< RingTraits, int32_t >;

  TestClientSet client {
    TestClientSet::Ring{5}, TestClientSet::Ring{7},
    { 1, 2 }, []() { return 1; } };

  SECTION( "preparing a set is to create an polynomial with encrypted coefficients" ) {
    auto const encryptedPolynomial = client.forServer();
    REQUIRE( encryptedPolynomial.size() == 3 );
    CHECK( encryptedPolynomial[ 0 ] ==
        TestClientSet::Cipher( TestClientSet::Ring{ 2 }, TestClientSet::Ring{ 10 } ) );
    CHECK( encryptedPolynomial[ 1 ] ==
        TestClientSet::Cipher( TestClientSet::Ring{ 2 }, TestClientSet::Ring{ -15 } ) );
  }
}
