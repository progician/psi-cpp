#pragma once

#include <CryptoCom/CyclicRing.hpp>
#include <functional>

namespace CryptoCom {

  template< typename RingTraits >
    struct ElGamal {
      using Ring = CyclicRing< RingTraits >;
      using RNG = std::function< Ring() >;

      struct Cipher {
        Ring const c[ 2 ];

        Cipher() = delete;
        Cipher( Ring const& c1, Ring const& c2 )
          : c { c1, c2 } {}

        Cipher operator +( Cipher const& other ) {
          return Cipher {
            c[ 0 ] * other.c[ 1 ],
            c[ 1 ] * other.c[ 2 ] };
        }

        Cipher operator +( Ring const& other ) {
          return Cipher {
            c[ 0 ],
            c[ 1 ] * Ring { RingTraits::Generator }.pow( other ) };
        }

        Cipher operator *( Ring const& other ) {
          return {
            c[ 0 ].pow( other ),
            c[ 1 ].pow( other ) };
        }
      };


      static std::tuple< Ring, Ring >
      newKeyPair( std::function< Ring() > rng ) {
        auto const secret = rng();
        return std::make_tuple( secret, Ring::Generator().pow( secret ) );
      }


      static Cipher
      encrypt( Ring const& key, Ring const& plainText, RNG rng ) {
        auto const random = rng();
        auto const a = key.pow( random );
        auto const b = Ring::Generator().pow( plainText );
        return Cipher{
          Ring::Generator().pow( random ),
          a * b };
      }


      static Ring decrypt( Ring const& key, Cipher const& encryptedMessage ) {
        auto const sharedSecret = encryptedMessage.c[ 0 ] ^ key;
        return encryptedMessage.c[ 1 ] / sharedSecret;
      }
    };

} // End of namespace 'CryptoCom'
