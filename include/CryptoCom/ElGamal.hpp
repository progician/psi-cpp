#pragma once

#include <CryptoCom/CyclicRing.hpp>
#include <functional>
#include <tuple>

namespace CryptoCom {

  template< typename RingTraits >
    struct ElGamal {
      using Ring = CyclicRing< RingTraits >;
      using RNG = std::function< Ring() >;
      using Cipher = std::tuple< Ring, Ring >;


      static std::tuple< Ring, Ring >
      KeyPairOf( std::function< Ring() > rng ) {
        auto const secret = rng();
        return std::make_tuple(
            secret,
            Ring::Generator() ^ secret );
      }


      static Cipher
      Encrypt( Ring const& key, Ring const& plainText, RNG rng ) {
        auto const random = rng();
        return Cipher{
          Ring::Generator() ^ random,
          ( key ^ random ) * plainText };
      }


      static Ring
      Decrypt( Ring const& key, Cipher const& encryptedMessage ) {
        auto const sharedSecret = std::get< 0 >( encryptedMessage ) ^ key;
        return std::get< 1 >( encryptedMessage ) / sharedSecret;
      }
    };

} // End of namespace 'CryptoCom'
