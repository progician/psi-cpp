#pragma once

#include <CryptoCom/CyclicRing.hpp>
#include <CryptoCom/elgamal.hpp>
#include <CryptoCom/polynomial.hpp>

#include <set>
#include <algorithm>

namespace CryptoCom {

  template< typename RingTraits >
    struct PrivateIntersection {
      using Ring = CyclicRing< RingTraits >;
      using EncryptionSystem = ElGamal< RingTraits >;
      using CipherType = typename EncryptionSystem::Cipher;
      using RNG = typename EncryptionSystem::RNG;

      template< typename InputIt >
        static std::vector< CipherType >
        encryptedPolynomial(
            InputIt first, InputIt last,
            Ring const& publicKey,
            RNG rng ) {
          auto const polynomial = poly::fromRoots(
              first, last,
              typename InputIt::value_type( -1 ),
              typename InputIt::value_type(  1 ) );

          std::vector< CipherType > result;
          for ( auto const& coeff : polynomial ) {
            result.push_back(
                EncryptionSystem::encrypt( publicKey, coeff, rng ) );
          }

          return result;
        }


      template< typename InputIt >
        static std::vector< CipherType >
        obliviousEvaluation(
            InputIt first, InputIt last,
            std::vector< CipherType > const& encryptedPolynomial,
            RNG rng ) {
          std::vector< CipherType > result;
          for ( auto current = first; current != last; ++current ) {
            auto const c = cryptoGroup( *current );
            auto const evaluated = poly::eval( c, encryptedPolynomial.begin(), encryptedPolynomial.end() );
            result.push_back( evaluated * rng() + c );
          }

          return result;
        }


      template< typename InputType,
                typename InputIt >
        static std::set< InputType >
        extractIntersection(
            InputIt first, InputIt last,
            std::vector< CipherType > const& evaluatedElements,
            Ring const& privateKey ) {

          std::vector< Ring > normalized(
              std::distance( first, last ), privateKey.group.random() );
          std::transform(
              first, last, normalized.begin(),
              [&]( InputType const& elem ) {
                auto const& cryptoGroup = privateKey.group;
                return cryptoGroup.g() ^ privateKey.group( InputType( elem ) );
              } );

          std::set< InputType > results;
          for ( auto const& e : evaluatedElements ) {
            auto const decryptedElem = EncryptionSystem::decrypt( privateKey, e );
            auto const it = std::find( normalized.begin(), normalized.end(), decryptedElem );
            if ( it != normalized.end() ) {
              auto originalIndex = std::distance( normalized.begin(), it );
              auto elem = first;
              auto originalValue = *( std::next( elem, originalIndex ) );
              results.insert( originalValue );
            }
          }

          return results;
        }
    };
} // CryptoCom
