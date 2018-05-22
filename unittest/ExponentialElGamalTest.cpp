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
  operator<<(std::ostream& ostr, CyclicRing<RingTraits> const& e) {
    ostr << e.ordinalIndex_;
    return ostr;
  }


  std::ostream&
  operator<<(std::ostream& ostr, ExponentialElGamal<RingTraits>::Cipher const& c) {
    ostr << "(" << c.components[0] << ", " << c.components[1] << ")";
    return ostr;
  }

} // CryptoCom


TEST_CASE("Exponential ElGamal encryption scheme") {
  using Ring = CryptoCom::CyclicRing<RingTraits>;
  using EncryptionScheme = CryptoCom::ExponentialElGamal<RingTraits>;


  SECTION("with a valid encryption key pair") {
    auto const keyPair = EncryptionScheme::KeyPairOf([]() { return Ring{5}; });
    EncryptionScheme::Ring privateKey, publicKey;
    std::tie(privateKey, publicKey) = keyPair;
    REQUIRE(privateKey == 5);
    REQUIRE(publicKey == 32);


    SECTION("encrypting done by an external (random) number results in a valid cipher") {
      auto const cipher = EncryptionScheme::Encrypt(publicKey, 2, []() { return Ring{3}; });
      REQUIRE(cipher == EncryptionScheme::Cipher(Ring{8}, Ring{568}));
    }


    SECTION("decrypting done by using the complementary key and a valid cipher") {
      auto const plainText = EncryptionScheme::Decrypt(privateKey, {8, 568});
      REQUIRE(plainText == 4);
    }


    SECTION("encryption is homomorphic to") {
      Ring const eight{8};
      Ring const twelve{12};
      Ring const twenty{20};
      Ring const ninetysix{8 * 12};

      std::list<Ring> additiveSequence{3, 6};
      EncryptionScheme::RNG sequenceFunction = [&additiveSequence]() {
        auto res = additiveSequence.front();
        additiveSequence.pop_front();
        return res;
      };

      auto const encryptedEight =
          EncryptionScheme::Encrypt(publicKey, eight, sequenceFunction);
      auto const encryptedTwelve =
          EncryptionScheme::Encrypt(publicKey, twelve, sequenceFunction);

      SECTION("adding to other cipher with adding the random 'salts' together") {
        additiveSequence = {9};
        auto const encryptedTwenty =
            EncryptionScheme::Encrypt( publicKey, twenty, sequenceFunction );
        REQUIRE(encryptedEight + encryptedTwelve == encryptedTwenty);
      }

      SECTION("adding to plaintext with keeping the random salt the same") {
        additiveSequence = {3};
        auto const encryptedTwenty =
            EncryptionScheme::Encrypt( publicKey, twenty, sequenceFunction );
        REQUIRE(encryptedEight + twelve == encryptedTwenty);
      }

      SECTION("adding the encrypted cipher to itself is the same as multiplying"
              "with plain number two") {
        const Ring sixteen{16};
        additiveSequence = {6};
        auto const encryptedSixteen =
            EncryptionScheme::Encrypt(publicKey, sixteen, sequenceFunction);
        REQUIRE(encryptedEight * 2 == encryptedSixteen);
      }

      SECTION("even in negative range") {
        additiveSequence = {3, 6, 9};
        auto const one = EncryptionScheme::Encrypt(privateKey, 1, sequenceFunction);
        auto const minus_one = EncryptionScheme::Encrypt(privateKey, -1, sequenceFunction);
        auto const zero = EncryptionScheme::Encrypt(privateKey, 0, sequenceFunction);
        REQUIRE(one + minus_one == zero);
      }
    }
  }
}
