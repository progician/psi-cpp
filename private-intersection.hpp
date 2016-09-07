#pragma once

#include "elgamal.hpp"
#include "finite-group.hpp"
#include "polynomial.hpp"
#include <set>
#include <stdint.h>
#include <algorithm>

namespace PrivateIntersection {

  template< typename InputIt >
    std::vector< ElGamal::Cipher >
    prepareLocalSet( InputIt first, InputIt last, fg::Elem const& publicKey )
    {
      auto count = std::distance( first, last );
      std::vector< fg::Elem > normalized( count, publicKey.group.random() );
      std::transform( first, last, normalized.begin(),
                      [&]( int64_t const& elem ) { return publicKey.group( elem ); } );

      auto const polynomial = poly::fromRoots(
        normalized.begin(), normalized.end(),
        publicKey.group.minusOne(),
        publicKey.group.plusOne() );

      std::vector< ElGamal::Cipher > result;
      for ( auto const& coeff : polynomial ) {
        result.push_back( ElGamal::encrypt( publicKey, coeff ) );
      }

      return result;
    }


  template< typename InputIt >
    std::vector< ElGamal::Cipher >
    obliviousEvaluation( InputIt first, InputIt last, std::vector< ElGamal::Cipher > const& encryptedPolynomial )
    {
      std::vector< ElGamal::Cipher > result;
      auto const& cryptoGroup = encryptedPolynomial.front().key.group;
      for ( auto current = first; current != last; ++current ) {
        auto const r = cryptoGroup.random();
        auto const c = cryptoGroup( *current );
        auto const evaluated = poly::eval< ElGamal::Cipher, fg::Elem, std::vector< ElGamal::Cipher >::const_iterator >( c,
                                           encryptedPolynomial.begin(), encryptedPolynomial.end() ) * r + c;
        result.push_back( evaluated );
      }

      return result;
    }

  template< typename InputType,
            typename InputIt >
    std::set< InputType >
    extractIntersection( InputIt first, InputIt last, std::vector< ElGamal::Cipher > const& evaluatedElements,
                         fg::Elem const& privateKey )
    {
      std::vector< fg::Elem > normalized( std::distance( first, last ), privateKey.group.random() );
      std::transform( first, last, normalized.begin(),
                      [&]( uint64_t const& elem )
                      {
                        auto const& cryptoGroup = privateKey.group;
                        return cryptoGroup.g() ^ privateKey.group( elem );
                      } );

      std::set< InputType > results;
      for ( auto const& e : evaluatedElements ) {
        auto const decryptedElem = ElGamal::decrypt( privateKey, e );
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
}
