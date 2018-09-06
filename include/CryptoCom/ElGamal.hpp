#pragma once

#include <CryptoCom/CyclicRing.hpp>
#include <array>
#include <functional>
#include <tuple>

namespace CryptoCom {

  template <typename RingTraits>
  struct ElGamal {
    using Ring = CyclicRing<RingTraits>;
    using RNG = std::function<Ring()>;
    using Cipher = std::array<Ring, 2>;

    static auto KeyPairOf(RNG rng) {
      auto const secret = rng();
      return std::make_tuple(secret, Ring::Generator() ^ secret);
    }


    static Cipher Encrypt(Ring const& key, Ring const& plainText, RNG rng) {
      auto const random = rng();
      return {{Ring::Generator() ^ random, (key ^ random) * plainText}};
    }


    static Ring Decrypt(Ring const& key, Cipher const& encryptedMessage) {
      auto const sharedSecret = encryptedMessage[0] ^ key;
      return encryptedMessage[1] / sharedSecret;
    }
  };

} // namespace CryptoCom
