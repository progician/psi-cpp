#pragma once

#include <CryptoCom/CyclicRing.hpp>
#include <CryptoCom/ExponentialElGamal.hpp>
#include <CryptoCom/Polynomial.hpp>

#include <algorithm>
#include <map>
#include <set>

namespace CryptoCom {
  namespace ObliviousEvaluation {

    template<
        typename RingType,
        typename InputType,
        typename EncryptionSystem =
          ExponentialElGamal< typename RingType::Traits > >
      class ClientSet {
      public:
        using Ring = RingType;
        using Cipher = typename EncryptionSystem::Cipher;
        using RNG = typename EncryptionSystem::RNG;

      private:
        RingType const privateKey_;
        std::map< RingType, InputType > const deciphered_;
        Polynomial< Cipher > const encryptedPolynomial_;

      public:
        ClientSet(
            RingType publicKey, RingType privateKey,
            std::set< InputType > const& privateSet,
            RNG rng )
          : privateKey_( privateKey )

          , deciphered_( [&privateSet]() {
              std::map< RingType, InputType > deciphered;
              for( auto const e : privateSet ) {
                deciphered[ EncryptionSystem::Decipher( e ) ] = e;
              }
              return deciphered;
            }() )

          , encryptedPolynomial_( [&privateSet, &publicKey, &rng]() {
              auto const inputPolynomial =
                Polynomial< RingType >::fromRoots(
                  privateSet.cbegin(), privateSet.cend() );

              std::vector< Cipher > encryptedCoefficients;
              std::transform(
                  inputPolynomial.cbegin(), inputPolynomial.cend(),
                  std::back_inserter( encryptedCoefficients ),
                  [&rng, &publicKey]( auto const& plain ) {
                    return EncryptionSystem::Encrypt(
                        publicKey, plain, rng ); } );
              return Polynomial< Cipher >{
                std::move( encryptedCoefficients ) };
            }() ) {}


        Polynomial< Cipher > forServer() const
        { return encryptedPolynomial_; }

        typename std::set< InputType >
        intersection(
            std::set< Cipher > const& evaluatedElements,
            RingType const& privateKey ) const {

          std::set< InputType > results;
          for( auto const& e : evaluatedElements ) {
            auto const decryptedElem =
              EncryptionSystem::Decrypt( privateKey, e );
            auto const it = deciphered_.find( decryptedElem );
            if ( it != deciphered_.end() )
              results.insert( it->second );
          }

          return results;
        }
      };


    template<
        typename RingType,
        typename InputType,
        typename EncryptionSystem =
          ExponentialElGamal< typename RingType::Traits > >
      class ServerSet {
        std::set< InputType > const privateSet_;

      public:
        ServerSet( std::set< InputType > const& elems )
          : privateSet_( elems ) {}

        using Cipher = typename EncryptionSystem::Cipher;
        using RNG = typename EncryptionSystem::RNG;

        std::set< Cipher >
        evaluate( Polynomial< Cipher > const& fromClient, RNG rng ) const {
          std::set< Cipher > result;
          for ( auto const localElem : privateSet_ ) {
            auto const c = cryptoroup( localElem );
            auto const evaluated = fromClient( c );
            result.insert( evaluated * rng() + c );
          }

          return result;
        }
      };
  } // ObliviousEvaluation 
} // CryptoCom
