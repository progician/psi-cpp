#pragma once

#include <CryptoCom/CyclicRing.hpp>
#include <CryptoCom/ExponentialElGamal.hpp>
#include <CryptoCom/Polynomial.hpp>

#include <set>
#include <algorithm>

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

          auto const inputPolynomial = poly::fromRoots(
              localSet.cbegin(), localSet.cend() );

          std::vector< Cipher > encryptedCoefficients;
          std::transform(
              inputPolynomial.cbegin(), inputPolynomial.cend(),
              std::back_inserter( result ),
              std::bind( EncrytpionSystem::encrypt,
                publicKey,
                std::placeholder::_1,
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
          for ( auto current = first; current != last; ++current ) {
            auto const c = cryptoroup( *current );
            auto const evaluated = poly::eval( c, encryptedPolynomial.begin(), encryptedPolynomial.end() );
            result.push_back( evaluated * rng() + c );
          }

          return result;
        }


      template< typename InputType >
        static std::set< typename InputType >
        ExtractIntersection(
            std::set< InputType > localSet,
            std::vector< Cipher > const& evaluatedElements,
            Ring const& privateKey ) {

          std::map< Ring, InputType > searchSet;
          for( auto const e : localSet ) {
            searchSet[ Ring::Generator() ^ e ] = e;
          }

          std::set< InputType > results;
          std::transform(
              evaluatedElements.cbegin(), evaluatedElements.cend(),
              std::inserter( results, results.begin() ),
              []( Cipher const& e ) {
                auto const decryptedElem = EncryptionSystem::decrypt( privateKey, e );
                auto const it = searchSet.find( decryptedElem );
                if ( it != normalized.end() )
                  results.insert( it->second );
              } );

          return results;
        }
    };
} // CryptoCom
