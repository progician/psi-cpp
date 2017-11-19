#include <CryptoCom/elgamal.hpp>

#include <tuple>
#include <boost/assert.hpp>
#include <iostream>

namespace ElGamal {

  Cipher::Cipher( fg::Elem const& c1, fg::Elem const& c2, fg::Elem const& k )
  : c { c1, c2 }
  , key( k ) { }

  Cipher&
  Cipher::operator+=( Cipher const& other )
  {
    BOOST_ASSERT( key == other.key );
    c[ 0 ] *= other.c[ 0 ];
    c[ 1 ] *= other.c[ 1 ];
    return *this;
  }


  Cipher&
  Cipher::operator+=( fg::Elem const& other )
  {
    c[ 1 ] *= key.group.g() ^ other;
    return *this;
  }

  Cipher&
  Cipher::operator*=( fg::Elem const& other )
  {
    c[ 0 ] ^= other;
    c[ 1 ] ^= other;
    return *this;
  }


  std::tuple< fg::Elem, fg::Elem >
  keygen( fg::Group const& cryptoGroup )
  {
    auto const secretKey = cryptoGroup.random();
    return std::make_tuple( secretKey, cryptoGroup.g() ^ secretKey );
  }


  Cipher
  encrypt( fg::Elem const& key, int64_t const plainText )
  {
    auto const r = key.group.random();
    auto const a = key ^ r;
    auto const b = key.group.g() ^ plainText;
    return Cipher(
      key.group.g() ^ r,
      a * b,
      key );
  }


  fg::Elem
  decrypt( fg::Elem const& key, Cipher const& encryptedMessage )
  {
    auto const sharedSecret = encryptedMessage.c[ 0 ] ^ key;
    return encryptedMessage.c[ 1 ] / sharedSecret;
  }

} // End of namespace 'ElGamal'
