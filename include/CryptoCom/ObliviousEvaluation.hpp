#pragma once

#include <CryptoCom/CyclicRing.hpp>
#include <CryptoCom/ExponentialElGamal.hpp>
#include <CryptoCom/Polynomial.hpp>

#include <algorithm>
#include <map>
#include <set>

namespace CryptoCom {

  template< typename RingTraits >
    struct ObliviousEvaluation {
      using Ring = CyclicRing< RingTraits >;
      using EncryptionSystem = ElGamal< RingTraits >;
      using Cipher = typename EncryptionSystem::Cipher;
      using RNG = typename EncryptionSystem::RNG;

      template< typename InputType >
        static Polynomial< Cipher >
        EncryptedPolynomial(
            std::set< InputType > const& localSet,
            Ring const& publicKey,
            RNG rng ) {

          auto const inputPolynomial = Polynomial< Ring >::fromRoots(
              localSet.cbegin(), localSet.cend() );

          std::vector< Cipher > encryptedCoefficients;
          std::transform(
              inputPolynomial.cbegin(), inputPolynomial.cend(),
              std::back_inserter( encryptedCoefficients ),
              std::bind( EncryptionSystem::Encrypt,
                publicKey,
                std::placeholders::_1,
                rng ) );
          return Polynomial< Cipher >( std::move( encryptedCoefficients ) );
        }


      template< typename InputType >
        static std::vector< Cipher >
        Evaluate(
            std::set< InputType > const& localSet,
            Polynomial<  Cipher > const& encryptedPolynomial,
            RNG rng ) {
          std::vector< Cipher > result;
          for ( auto const localElem : localSet ) {
            auto const c = cryptoroup( localElem );
            auto const evaluated = encryptedPolynomial( c );
            result.push_back( evaluated * rng() + c );
          }

          return result;
        }


      template< typename InputType >
        static typename std::set< InputType >
        ExtractIntersection(
            std::set< InputType > localSet,
            std::vector< Cipher > const& evaluatedElements,
            Ring const& privateKey ) {

          std::map< Ring, InputType > searchSet;
          for( auto const e : localSet ) {
            searchSet[ Ring::Generator() ^ e ] = e;
          }

          std::set< InputType > results;
          for( auto const& e : evaluatedElements ) {
            auto const decryptedElem = EncryptionSystem::decrypt( privateKey, e );
            auto const it = searchSet.find( decryptedElem );
            if ( it != searchSet.end() )
              results.insert( it->second );
          }

          return results;
        }
    };
} // CryptoCom
