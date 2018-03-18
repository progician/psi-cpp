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
  using PSI = CryptoCom::ObliviousEvaluation< RingTraits >;
  using Ring = PSI::Ring;

  SECTION( "preparing a set is to create an polynomial with encrypted coefficients" ) {
    std::set< int32_t > originalSet { 1, 2 };
    auto const encryptedPolynomial = PSI::EncryptedPolynomial(
        originalSet, Ring{ 5 }, 
        []() { return 1; } );
    REQUIRE( encryptedPolynomial.size() == 3 );
    CHECK( encryptedPolynomial[ 0 ] ==
        PSI::EncryptionSystem::Cipher( Ring{ 2 }, Ring{ 10 } ) );
    CHECK( encryptedPolynomial[ 1 ] ==
        PSI::EncryptionSystem::Cipher( Ring{ 2 }, Ring{ -15 } ) );
  }
}
