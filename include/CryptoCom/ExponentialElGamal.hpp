#pragma once

#include <CryptoCom/CyclicRing.hpp>
#include <CryptoCom/ElGamal.hpp>
#include <array>
#include <stdexcept>
#include <functional>
#include <tuple>

namespace CryptoCom {

  template< typename RingTraits >
    struct ExponentialElGamal {
      using Ring = CyclicRing< RingTraits >;
      using RNG = std::function< Ring() >;
      using Base = ElGamal< RingTraits >;

      struct Cipher {
        std::array<Ring, 2> components;
        
        Cipher(typename Base::Cipher const& cipher)
          : components(cipher) {}

        Cipher(Ring const& c0, Ring const& c1)
          : components(typename Base::Cipher{{c0, c1}}) {
            if (c0 == Ring::Zero() || c1 == Ring::Zero())
              throw std::out_of_range("ElGamal cipher can't contain 0");
          }

        Cipher operator+(Cipher const& other) const {
          return {
            components[0] * other.components[0],
            components[1] * other.components[1]
          };
        }

        Cipher operator+(Ring const& other) const {
          return {components[0], components[1] * (Ring::Generator() ^ other)};
        }

        Cipher operator*(Ring const& other) const {
          return {components[0] ^ other, components[1] ^ other};
        }

        bool operator==(Cipher const& other) const {
          return components == other.components;
        }

        bool operator<(Cipher const& other) const {
          if (components[0] == other.components[0])
            return components[1] < other.components[1];
          return components[0] < other.components[0];
        }
      };


      static Ring Decipher(Ring const& e)
      { return Ring::Generator() ^ e; }


      static std::tuple<Ring, Ring>
      KeyPairOf(RNG rng) { return ElGamal<RingTraits>::KeyPairOf(rng); }


      template<typename IntegralType>
        static Cipher
        Encrypt(Ring const& key, IntegralType plainText, RNG rng) {
          return ElGamal<RingTraits>::Encrypt(
              key, Ring::Generator() ^ plainText, rng);
        }


      static Ring
      Decrypt(Ring const& key, Cipher const& encryptedMessage) {
        return ElGamal<RingTraits>::Decrypt(key, encryptedMessage.components);
      }
    };

} // End of namespace 'CryptoCom'

