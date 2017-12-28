#include <CryptoCom/elgamal.hpp>
#include <catch/catch.hpp>

struct RingTraits {
  using PrimaryType = int32_t;
  using EscalationType = int64_t;
  static constexpr PrimaryType Order = 0x800000;
  static constexpr PrimaryType Generator = 2;
  static constexpr PrimaryType AdditiveIdentity = 0;
  static constexpr PrimaryType MultiplicativeIdentity = 1;
};


namespace CryptoCom {
  std::ostream&
  operator <<( std::ostream& ostr, CyclicRing< RingTraits > const& e ) {
    ostr << e.ordinalIndex_;
    return ostr;
  }

} // CryptoCom


TEST_CASE( "Exponential EGamal encryption scheme" ) {
  using Ring = CryptoCom::CyclicRing< RingTraits >;
  using EncryptionScheme = CryptoCom::ElGamal< RingTraits >;


  SECTION( "generating encryption key pair" ) {
    auto const keyPair = EncryptionScheme::newKeyPair(
        []() { return Ring { 1 }; } );
    REQUIRE( std::get< 0 >( keyPair ) == Ring { 1 } );
    REQUIRE( std::get< 1 >( keyPair ) == Ring { 2 } );
  }


  SECTION( "encrypting done by an external (random ) number results in a valid cipher" ) {
    auto const cipher = EncryptionScheme::encrypt(
        Ring { 1 }, Ring { 2 },
        []() { return Ring { 2 }; } );
    REQUIRE( cipher.c[ 0 ] == Ring { 4 } );
    REQUIRE( cipher.c[ 1 ] == Ring { 4 } );
  }
}
