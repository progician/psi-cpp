#pragma once

#include "finite-group.hpp"
#include <boost/operators.hpp>


namespace ElGamal {

  struct Cipher
  : public boost::addable< Cipher, Cipher >
  , public boost::addable< Cipher, fg::Elem >
  , public boost::multipliable< Cipher, fg::Elem >
  {
    fg::Elem c[ 2 ];
    fg::Elem key;

    Cipher() = delete;
    Cipher( fg::Elem const& c1, fg::Elem const& c2, fg::Elem const& k );

    Cipher& operator +=( Cipher const& other );
    Cipher& operator +=( fg::Elem const& other );
    Cipher& operator *=( fg::Elem const& other );
  };


  std::tuple< fg::Elem, fg::Elem >
  keygen( fg::Group const& cryptoGroup );

  Cipher
  encrypt( fg::Elem const& key, fg::Elem const& plainText );

  fg::Elem
  decrypt( fg::Elem const& key, Cipher const& encryptedMessage );

} // End of namespace 'ElGamal'
