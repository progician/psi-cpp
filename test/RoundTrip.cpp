#include <CryptoCom/ExponentialElGamal.hpp>
#include <CryptoCom/ObliviousEvaluation.hpp>

#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>

#include <iostream>
#include <random>
#include <set>
#include <stdexcept>


std::ostream& operator<<(std::ostream& ostr, std::set<int32_t> const& v) {
  std::ostream_iterator<int32_t> it(ostr, " ");
  std::copy(v.cbegin(), v.cend(), it);
	return ostr;
}



struct RingTraits {
  using PrimaryType = int64_t;
  using EscalationType = int64_t;
  using CoefficientType = int64_t;
  static constexpr PrimaryType Order{2250635938};
  static constexpr PrimaryType Generator{3};
  static constexpr PrimaryType AdditiveIdentity{0};
  static constexpr PrimaryType MultiplicativeIdentity{1};
};


using Ring = CryptoCom::CyclicRing<RingTraits>;
using Encryption = CryptoCom::ExponentialElGamal<RingTraits>;
using ClientSet =
    CryptoCom::ObliviousEvaluation::ClientSet<Ring, int32_t, Encryption>;
using ServerSet =
    CryptoCom::ObliviousEvaluation::ServerSet<
        Ring, int32_t, CryptoCom::ExponentialElGamal<RingTraits>>;

namespace std {
  std::ostream& operator<<(std::ostream& lhs, std::set<int32_t> const& rhs) {
    lhs << "{ ";
    for (auto e: rhs) {
      lhs << e << " ";
    }
    lhs << "}";
    return lhs;
  }
}


SCENARIO("Calculate intersection between two sets with ElGamal encryption system") {
  std::default_random_engine generator;
  std::uniform_int_distribution<int64_t> distribution{1, RingTraits::Order};
  auto rng = [&generator, &distribution]() -> Ring { return distribution(generator); };

  Ring public_key, private_key;
  std::tie(public_key, private_key) = Encryption::KeyPairOf(rng);

  GIVEN("a client and a server set") {
    ClientSet client_set{public_key, private_key, {2, 4, 6}, rng};
    ServerSet server_set{{3, 6, 9}};

    WHEN("calculating the encrypted polynomial and evaluating it on the server side") {
      auto const polynomial = client_set.forServer();
      auto const evaluated = server_set.evaluate(polynomial, rng);

      THEN("the client can extract the intersection of the two sets") {
        auto const intersection = client_set.intersection(evaluated, private_key);
        REQUIRE(intersection == (std::set<int32_t>{6}));
      }
    }
  }
}