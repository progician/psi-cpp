#include <CryptoCom/ExponentialElGamal.hpp>
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


  struct SmallRingTraits {
    using PrimaryType = int32_t;
    using EscalationType = int64_t;
    using CoefficientType = int64_t;

    static constexpr PrimaryType Order{19};
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


  std::ostream& operator<<(
      std::ostream& ostr, ExponentialElGamal<RingTraits>::Cipher const& c) {
    ostr << "(" << c.components[0] << ", " << c.components[1] << ")";
    return ostr;
  }
} // namespace CryptoCom


TEST_CASE("The exponential ElGamal encryption scheme") {
  using Ring = CryptoCom::CyclicRing<RingTraits>;
  using ExpElGamal = CryptoCom::ExponentialElGamal<RingTraits>;

  SECTION("with a valid encryption key pair") {
    auto const keyPair = ExpElGamal::KeyPairOf([]() { return Ring{5}; });
    ExpElGamal::Ring private_key, public_key;
    std::tie(private_key, public_key) = keyPair;
    REQUIRE(private_key == 5);
    REQUIRE(public_key == 32);


    SECTION("encrypting done by an external (random) number results in a valid "
            "cipher") {
      auto const cipher =
          ExpElGamal::Encrypt(public_key, 2, []() { return Ring{3}; });
      REQUIRE(cipher == ExpElGamal::Cipher(Ring{8}, Ring{568}));
    }


    SECTION(
        "decrypting done by using the complementary key and a valid cipher") {
      auto const plainText = ExpElGamal::Decrypt(private_key, {8, 568});
      REQUIRE(plainText == 4);
    }


    SECTION("encryption is homomorphic to") {
      Ring const eight{8};
      Ring const twelve{12};
      Ring const twenty{20};
      Ring const ninetysix{8 * 12};

      std::list<Ring> seq{3, 6};
      ExpElGamal::RNG rng = [&seq]() {
        auto res = seq.front();
        seq.pop_front();
        return res;
      };

      auto const cipher_eight = ExpElGamal::Encrypt(public_key, eight, rng);
      auto const cipher_twelve = ExpElGamal::Encrypt(public_key, twelve, rng);

      SECTION(
          "adding to other cipher with adding the random 'salts' together") {
        seq = {9};
        auto const cipher_twenty = ExpElGamal::Encrypt(public_key, twenty, rng);
        REQUIRE(cipher_eight + cipher_twelve == cipher_twenty);
      }

      SECTION("adding a number and its negative, so it results in zero") {
        seq = {3, 6, 9};
        auto const cipher_one = ExpElGamal::Encrypt(private_key, 1, rng);
        auto const cipher_minus_one = ExpElGamal::Encrypt(public_key, -1, rng);
        auto const cipher_zero = ExpElGamal::Encrypt(private_key, 0, rng);
        REQUIRE(cipher_one + cipher_minus_one == cipher_zero);
      }

      SECTION("adding to plaintext with keeping the random salt the same") {
        seq = {3};
        auto const cipher_twenty = ExpElGamal::Encrypt(public_key, twenty, rng);
        REQUIRE(cipher_eight + twelve == cipher_twenty);
      }

      SECTION("adding the encrypted cipher to itself is the same as multiplying"
              "with plain number two") {
        seq = {6};
        auto const cipher_sixteen = ExpElGamal::Encrypt(public_key, 16, rng);
        REQUIRE(cipher_eight * 2 == cipher_sixteen);
      }
    }
  }
}
