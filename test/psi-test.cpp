#include <CryptoCom/ExponentialElGamal.hpp>
#include <CryptoCom/ObliviousEvaluation.hpp>

#include <iostream>
#include <random>
#include <set>


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


int main(int, char**) {
  std::default_random_engine generator;
  std::uniform_int_distribution<int64_t> distribution{1, RingTraits::Order};
  auto rng = [&generator, &distribution]() ->Ring { return distribution(generator); };

  Ring public_key, private_key;
  std::tie(public_key, private_key) = Encryption::KeyPairOf(rng);

  // Send to server
  ClientSet client_set{public_key, private_key, {2, 4, 6}, rng};
  auto const polynomial = client_set.forServer();

  // Server computation
  ServerSet server_set{{3, 6, 9}};
  auto const evaluated = server_set.evaluate(polynomial, rng);

  // Receive from server
  auto const intersection = client_set.intersection(evaluated, private_key);
  std::cout << intersection << std::endl;

	return 0;
}
