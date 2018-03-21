#pragma once

#include <CryptoCom/CyclicRing.hpp>
#include <CryptoCom/ElGamal.hpp>
#include <functional>
#include <tuple>

namespace CryptoCom {

  template< typename RingTraits >
    struct ExponentialElGamal {
      using Ring = CyclicRing< RingTraits >;
      using RNG = std::function< Ring() >;
      using Base = ElGamal< RingTraits >;

      struct Cipher {
        std::tuple< Ring, Ring > components;
        
        Cipher( typename Base::Cipher const& cipher )
          : components( cipher ) {}

        Cipher( Ring const& c0, Ring const& c1 )
          : components( typename Base::Cipher( c0, c1 ) ) {}

        Cipher operator+( Cipher const& other ) const {
          return Cipher{
            std::get< 0 >( components ) * std::get< 0 >( other.components ),
            std::get< 1 >( components ) * std::get< 1 >( other.components ) };
        }

        Cipher operator+( Ring const& other ) const {
          return Cipher{
            std::get< 0 >( components ),
            std::get< 1 >( components ) * ( Ring::Generator() ^ other ) };
        }

        Cipher operator*( Ring const& other ) const {
          return Cipher{
            std::get< 0 >( components ) ^ other,
            std::get< 1 >( components ) ^ other };
        }

        bool operator==( Cipher const& other ) const {
          return components == other.components;
        }
      };


      static Ring Decipher( Ring const& e )
      { return Ring::Generator() ^ e; }


      static std::tuple< Ring, Ring >
      KeyPairOf( std::function< Ring() > rng ) {
        return ElGamal< RingTraits >::KeyPairOf( rng );
      }


      static Cipher
      Encrypt( Ring const& key, Ring const& plainText, RNG rng ) {
        return ElGamal< RingTraits >::Encrypt(
            key, Ring::Generator() ^ plainText, rng );
      }


      static Ring
      Decrypt( Ring const& key, Cipher const& encryptedMessage ) {
        return ElGamal< RingTraits >::Decrypt( key, encryptedMessage.components );
      }
    };

} // End of namespace 'CryptoCom'

