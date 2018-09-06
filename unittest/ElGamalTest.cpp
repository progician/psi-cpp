#include <CryptoCom/ElGamal.hpp>
#include <catch/catch.hpp>
#include <list>

namespace {

  struct RingTraits {
    using PrimaryType = int32_t;
    using EscalationType = int64_t;
    using CoefficientType = int64_t;

    static constexpr PrimaryType Order{1483};
    static constexpr PrimaryType Generator{2};
    static constexpr PrimaryType AdditiveIdentity{0};
    static constexpr PrimaryType MultiplicativeIdentity{1};
  };

} // namespace


namespace CryptoCom {

  std::ostream& operator<<(
      std::ostream& ostr, CyclicRing<RingTraits> const& e) {
    ostr << e.ordinalIndex_;
    return ostr;
  }

} // namespace CryptoCom


CryptoCom::ElGamal<RingTraits>::Cipher operator*(
    CryptoCom::ElGamal<RingTraits>::Cipher a,
    CryptoCom::ElGamal<RingTraits>::Cipher b) {
  return {{a[0] * b[0], a[1] * b[1]}};
}


TEST_CASE("ElGamal encryption scheme") {
  using Ring = CryptoCom::CyclicRing<RingTraits>;
  using EncryptionScheme = CryptoCom::ElGamal<RingTraits>;


  SECTION("with a valid encryption key pair") {
    EncryptionScheme::Ring privateKey, publicKey;
    std::tie(privateKey, publicKey) =
        EncryptionScheme::KeyPairOf([]() { return Ring{5}; });
    REQUIRE(privateKey == 5);
    REQUIRE(publicKey == 32);


    SECTION("encrypting done by an external (random) number results in a valid "
            "cipher") {
      auto const cipher =
          EncryptionScheme::Encrypt(publicKey, 2, []() { return Ring{3}; });
      REQUIRE(cipher[0] == 8);
      REQUIRE(cipher[1] == 284);
    }


    SECTION(
        "decrypting done by using the complementary key and a valid cipher") {
      auto const plainText = EncryptionScheme::Decrypt(privateKey, {{8, 284}});
      REQUIRE(plainText == 2);
    }


    SECTION("encryption is homomorphic to multiplication") {
      std::list<Ring> additiveSequence{3, 6, 9};
      EncryptionScheme::RNG sequenceFunction = [&additiveSequence]() {
        auto res = additiveSequence.front();
        additiveSequence.pop_front();
        return res;
      };

      auto const eight =
          EncryptionScheme::Encrypt(publicKey, 8, sequenceFunction);
      auto const twelve =
          EncryptionScheme::Encrypt(publicKey, 12, sequenceFunction);
      auto const ninetySix =
          EncryptionScheme::Encrypt(publicKey, 96, sequenceFunction);
      REQUIRE(eight * twelve == ninetySix);
    }
  }
}
