#include <CryptoCom/ElGamal.hpp>
#include <catch/catch.hpp>
#include <list>

struct RingTraits {
  using PrimaryType = int32_t;
  using EscalationType = int64_t;
  using CoefficientType = int64_t;

  static constexpr PrimaryType Order = 1483;
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


CryptoCom::ElGamal< RingTraits >::Cipher
Multiply(
    CryptoCom::ElGamal< RingTraits >::Cipher a,
    CryptoCom::ElGamal< RingTraits >::Cipher b ) {
  return std::make_tuple(
      std::get< 0 >( a ) * std::get< 0 >( b ),
      std::get< 1 >( a ) * std::get< 1 >( b ) );
}


TEST_CASE( "Exponential ElGamal encryption scheme" ) {
  using Ring = CryptoCom::CyclicRing< RingTraits >;
  using EncryptionScheme = CryptoCom::ElGamal< RingTraits >;


  SECTION( "with a valid encryption key pair" ) {
    auto const keyPair = EncryptionScheme::KeyPairOf(
        []() { return Ring { 5 }; } );
    auto const privateKey = std::get< 0 >( keyPair );
    auto const publicKey = std::get< 1 >( keyPair );
    REQUIRE( privateKey == Ring { 5 } );
    REQUIRE( publicKey == Ring { 32 } );


    SECTION( "encrypting done by an external (random) number results in a valid cipher" ) {
      auto const cipher = EncryptionScheme::Encrypt(
          publicKey, Ring { 2 },
          []() { return Ring { 3 }; } );
      REQUIRE( cipher == std::make_tuple( Ring{ 8 }, Ring{ 284 } ) );
    }


    SECTION( "decrypting done by using the complementary key and a valid cipher" ) {
      auto const plainText = EncryptionScheme::Decrypt(
          privateKey,
          EncryptionScheme::Cipher{
            Ring{ 8 }, Ring{ 284 } } );
      REQUIRE( plainText == Ring{ 2 } );
    }


    SECTION( "encryption is homomorphic to multiplication" ) {
      Ring constexpr eight { 8 };
      Ring constexpr twelve { 12 };
      Ring constexpr ninetySix { 96 };

      std::list< Ring > additiveSequence = {
        Ring{ 3 }, Ring{ 6 }, Ring{ 9 } };

      EncryptionScheme::RNG sequenceFunction = [&additiveSequence]() {
        auto res = additiveSequence.front();
        additiveSequence.pop_front();
        return res;
      };

      auto const encryptedEight = EncryptionScheme::Encrypt( publicKey, eight, sequenceFunction );
      auto const encryptedTwelve = EncryptionScheme::Encrypt( publicKey, twelve, sequenceFunction );
      auto const encryptedNinetySix = EncryptionScheme::Encrypt( publicKey, ninetySix, sequenceFunction );

      REQUIRE( Multiply( encryptedEight, encryptedTwelve ) == encryptedNinetySix );
    }
  }
}
