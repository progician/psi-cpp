#include <CryptoCom/ExponentialElGamal.hpp>
#include <catch/catch.hpp>
#include <list>


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


TEST_CASE( "Exponential ElGamal encryption scheme" ) {
  using Ring = CryptoCom::CyclicRing< RingTraits >;
  using EncryptionScheme = CryptoCom::ExponentialElGamal< RingTraits >;


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
      REQUIRE( cipher == std::make_tuple( Ring{ 8 }, Ring{ 568 } ) );
    }


    SECTION( "decrypting done by using the complementary key and a valid cipher" ) {
      auto const plainText = EncryptionScheme::Decrypt(
          privateKey,
          EncryptionScheme::Cipher{
            Ring{ 8 }, Ring{ 568 } } );
      REQUIRE( plainText == Ring{ 4 } );
    }


    SECTION( "encryption is homomorphic to" ) {
      Ring constexpr eight { 8 };
      Ring constexpr twelve { 12 };
      Ring constexpr twenty { 20 };

      std::list< Ring > additiveSequence = { Ring{ 3 }, Ring{ 6 } };

      EncryptionScheme::RNG sequenceFunction = [&additiveSequence]() {
        auto res = additiveSequence.front();
        additiveSequence.pop_front();
        return res;
      };

      auto const encryptedEight = EncryptionScheme::Encrypt( publicKey, eight, sequenceFunction );
      auto const encryptedTwelve = EncryptionScheme::Encrypt( publicKey, twelve, sequenceFunction );

      SECTION( "adding to other cipher with adding the random 'salts' together" ) {
        additiveSequence = std::list< Ring >{ Ring{ 9 } };
        auto const encryptedTwenty = EncryptionScheme::Encrypt( publicKey, twenty, sequenceFunction );
        REQUIRE( encryptedEight + encryptedTwelve == encryptedTwenty );
      }

      SECTION( "adding to plaintext with keeping the random salt the same" ) {
        additiveSequence = std::list< Ring >{ Ring{ 3 } };
        auto const encryptedTwenty = EncryptionScheme::Encrypt( publicKey, twenty, sequenceFunction );
        REQUIRE( encryptedEight + twelve == encryptedTwenty );
      }
    }
  }
}
