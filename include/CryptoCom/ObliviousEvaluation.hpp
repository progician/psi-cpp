#pragma once

#include <CryptoCom/CyclicRing.hpp>
#include <CryptoCom/ExponentialElGamal.hpp>
#include <CryptoCom/Polynomial.hpp>

#include <algorithm>
#include <map>
#include <set>

namespace CryptoCom {
  namespace ObliviousEvaluation {

    template< typename RingTraits, typename InputType >
      class ClientSet {
      public:
        using Ring = CyclicRing< RingTraits >;
        using EncryptionSystem = ElGamal< RingTraits >;
        using Cipher = typename EncryptionSystem::Cipher;
        using RNG = typename EncryptionSystem::RNG;

      private:
        Ring const privateKey_;
        std::map< Ring, InputType > const matchingExponentials_;
        Polynomial< Cipher > const encryptedPolynomial_;

      public:
        ClientSet(
            Ring publicKey, Ring privateKey,
            std::set< InputType > const& privateSet,
            RNG rng )
          : privateKey_( privateKey )

          , matchingExponentials_( [&privateSet]() {
              std::map< Ring, InputType > exponentialToElem;
              for( auto const e : privateSet ) {
                exponentialToElem[ Ring::Generator() ^ e ] = e;
              }
              return exponentialToElem;
            }() )

          , encryptedPolynomial_( [&privateSet, &publicKey, &rng]() {
              auto const inputPolynomial = Polynomial< Ring >::fromRoots(
                  privateSet.cbegin(), privateSet.cend() );

              std::vector< Cipher > encryptedCoefficients;
              std::transform(
                  inputPolynomial.cbegin(), inputPolynomial.cend(),
                  std::back_inserter( encryptedCoefficients ),
                  [&rng, &publicKey]( auto const& plain ) {
                    return EncryptionSystem::Encrypt( plain, publicKey, rng ); } );
              return Polynomial< Cipher >{ std::move( encryptedCoefficients ) };
            }() ) {}


        Polynomial< Cipher > forServer() const { return encryptedPolynomial_; }

        typename std::set< InputType >
        intersection(
            std::set< Cipher > const& evaluatedElements,
            Ring const& privateKey ) {

          std::set< InputType > results;
          for( auto const& e : evaluatedElements ) {
            auto const decryptedElem = EncryptionSystem::decrypt( privateKey, e );
            auto const it = matchingExponentials_.find( decryptedElem );
            if ( it != matchingExponentials_.end() )
              results.insert( it->second );
          }

          return results;
        }
      };


    template< typename RingTraits, typename InputType >
      class ServerSet {
        std::set< InputType > const privateSet_;

      public:
        ServerSet( std::set< InputType > const& elems ) : privateSet_( elems ) {}

        using EncryptionSystem = ElGamal< RingTraits >;
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
